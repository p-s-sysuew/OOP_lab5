#include "../include/functions.h"

#include <new>
#include <algorithm>
#include <iostream>


// ВНУТРЕННЯЯ РЕАЛИЗАЦИЯ FixedBlockResource


struct FixedBlockResource::Impl {

    // Указатель на начало пула памяти
    char* buffer;

    // Общий размер пула
    std::size_t poolSize;

    // Список свободных блоков (смещение, размер)
    std::list<std::pair<std::size_t, std::size_t>> freeList;

    // Список выделенных блоков
    std::list<std::pair<std::size_t, std::size_t>> allocList;

    Impl(std::size_t size) : poolSize(size) {
        buffer = static_cast<char*>(::operator new(size));
        freeList.emplace_back(0, size);
    }

    ~Impl() {
        ::operator delete(buffer);
    }

    static constexpr std::size_t NPOS = static_cast<std::size_t>(-1);

    // Поиск подходящего участка памяти
    std::size_t findBlock(std::size_t bytes, std::size_t alignment, std::size_t& offset) {

        for (auto it = freeList.begin(); it != freeList.end(); ++it) {

            std::size_t blockOffset = it->first;
            std::size_t blockSize = it->second;

            void* rawPtr = buffer + blockOffset;
            std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(rawPtr);
            std::uintptr_t aligned = (addr + (alignment - 1)) & ~(alignment - 1);

            std::size_t shift = aligned - addr;

            if (blockSize < shift + bytes)
                continue;

            offset = blockOffset + shift;

            std::size_t before = shift;
            std::size_t after = blockSize - shift - bytes;

            freeList.erase(it);

            if (before > 0)
                freeList.emplace_back(blockOffset, before);

            if (after > 0)
                freeList.emplace_back(offset + bytes, after);

            // Сортировка и слияние блоков
            freeList.sort();

            for (auto i = freeList.begin(); i != freeList.end();) {
                auto next = std::next(i);
                if (next != freeList.end() &&
                    i->first + i->second == next->first) {
                    i->second += next->second;
                    freeList.erase(next);
                } else {
                    ++i;
                }
            }

            return offset;
        }

        return NPOS;
    }

    // Возврат памяти в пул
    void releaseBlock(std::size_t offset, std::size_t bytes) {

        freeList.emplace_back(offset, bytes);
        freeList.sort();

        for (auto i = freeList.begin(); i != freeList.end();) {
            auto next = std::next(i);
            if (next != freeList.end() &&
                i->first + i->second == next->first) {
                i->second += next->second;
                freeList.erase(next);
            } else {
                ++i;
            }
        }
    }
};


//  FixedBlockResource


FixedBlockResource::FixedBlockResource(std::size_t poolSize)
    : impl_(new Impl(poolSize)) {}

FixedBlockResource::~FixedBlockResource() {

    // Сообщаем о неосвобождённой памяти
    if (!impl_->allocList.empty()) {
        std::cerr << "\n[MemoryResource] Обнаружена утечка памяти!\n";
        for (auto& block : impl_->allocList) {
            std::cerr << "Смещение: " << block.first
                      << ", Размер: " << block.second << " байт\n";
        }
    }

    delete impl_;
}

void* FixedBlockResource::do_allocate(std::size_t bytes, std::size_t alignment) {

    std::size_t offset = 0;
    std::size_t result = impl_->findBlock(bytes, alignment, offset);

    if (result == Impl::NPOS)
        throw std::bad_alloc();

    impl_->allocList.emplace_back(offset, bytes);
    return impl_->buffer + offset;
}

void FixedBlockResource::do_deallocate(void* ptr, std::size_t bytes, std::size_t) {

    std::uintptr_t base = reinterpret_cast<std::uintptr_t>(impl_->buffer);
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(ptr);

    // Проверка принадлежности пулу
    if (addr < base || addr >= base + impl_->poolSize)
        return;

    std::size_t offset = addr - base;

    auto it = std::find_if(impl_->allocList.begin(), impl_->allocList.end(),
        [offset](auto& elem) {
            return elem.first == offset;
        });

    if (it != impl_->allocList.end())
        impl_->allocList.erase(it);

    impl_->releaseBlock(offset, bytes);
}

bool FixedBlockResource::do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
}
