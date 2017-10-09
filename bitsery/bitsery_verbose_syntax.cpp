//MIT License
//
//Copyright (c) 2017 Mindaugas Vinkelis
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "testing_core/test.h"
#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>
#include <bitsery/traits/string.h>

namespace bitsery {

    template<typename S>
    void serialize(S &s, MyTypes::Vec3 &o) {
        s.value4b(o.x);
        s.value4b(o.y);
        s.value4b(o.z);
    }

    template<typename S>
    void serialize(S &s, MyTypes::Weapon &o) {
        s.text1b(o.name, 10);
        s.value2b(o.damage);
    }

    template<typename S>
    void serialize(S &s, MyTypes::Monster &o) {
        //forward/backward compatibility for monsters
        s.value1b(o.color);
        s.value2b(o.mana);
        s.value2b(o.hp);
        s.object(o.equipped);
        s.object(o.pos);
        s.container(o.path, 10);
        s.container(o.weapons, 10);
        s.container1b(o.inventory, 10);
        s.text1b(o.name, 10);
    }
}

using Buffer = std::vector<uint8_t>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using Writer = bitsery::AdapterWriter<OutputAdapter, bitsery::DefaultConfig>;
using Reader = bitsery::AdapterReader<InputAdapter, bitsery::DefaultConfig>;

class BitseryArchiver : public ISerializerTest {
public:

    Buf serialize(const std::vector<MyTypes::Monster> &data) override {
        _buf.clear();

        bitsery::Serializer<OutputAdapter> ser(OutputAdapter{_buf});
        ser.container(data, 100000000);
        auto& bw = bitsery::AdapterAccess::getWriter(ser);
        bw.flush();
        return Buf{std::addressof(*std::begin(_buf)), bw.writtenBytesCount()};
    }

    void deserialize(Buf buf, std::vector<MyTypes::Monster> &res) override {

        bitsery::BasicDeserializer<Reader> des(InputAdapter{_buf.begin(), buf.bytesCount});
        des.container(res, 100000000);
    }
private:
    Buffer _buf{};
};

int main () {
    BitseryArchiver test{};
    runTest("bitsery verbose syntax\n\tbuffer: std::vector<uint8_t>", test, MONSTERS, SAMPLES);
    return 0;
}
