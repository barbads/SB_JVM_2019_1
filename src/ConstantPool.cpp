#include <ConstantPool.hpp>

ConstantPool::ConstantPool(std::ifstream *file) {
    file->seekg(0);
    file->seekg(8);
    char constant_pool[2];
    auto ss = std::stringstream();
    file->read(constant_pool, 2);
    for (auto i = 0; i < 2; i++) {
        ss << static_cast<int>(static_cast<unsigned char>(constant_pool[i]));
    }

    const_pool_count = stoi(ss.str());
    std::cout << const_pool_count << std::endl;
}