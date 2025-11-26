#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <memory_resource>
#include <list>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <utility>

//  - один заранее выделенный блок памяти
//  - информация о выделенных блоках хранится в std::list
//  - освобождённая память может использоваться повторно
//  - при уничтожении — вывод информации о неосвобождённых блоках

class FixedBlockResource : public std::pmr::memory_resource {
public:
    // Конструктор — создаёт пул памяти заданного размера (в байтах)
    explicit FixedBlockResource(std::size_t poolSize = 1024);

    // Деструктор — освобождает память и сообщает об утечках
    ~FixedBlockResource() override;

    FixedBlockResource(const FixedBlockResource&) = delete;
    FixedBlockResource& operator=(const FixedBlockResource&) = delete;

private:
    // Выделение памяти
    void* do_allocate(std::size_t bytes, std::size_t alignment) override;

    // Освобождение памяти
    void do_deallocate(void* ptr, std::size_t bytes, std::size_t alignment) override;

    // Проверка равенства ресурсов памяти
    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override;

    struct Impl;
    Impl* impl_;
};

// двусвязный список
template <typename T>
class PmrList {
public:
    using value_type = T;
    using allocator_type = std::pmr::polymorphic_allocator<T>;

    explicit PmrList(std::pmr::memory_resource* resource = std::pmr::get_default_resource());
    ~PmrList();

    PmrList(const PmrList&) = delete;
    PmrList& operator=(const PmrList&) = delete;

    PmrList(PmrList&& other) noexcept;
    PmrList& operator=(PmrList&& other) noexcept;

    // Добавление элементов
    void push_back(const T& value);
    void push_front(const T& value);

    // Удаление элементов
    void pop_back();
    void pop_front();

    // Очистка
    void clear();

    // Размер
    std::size_t size() const noexcept;

    // итератор
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() noexcept : node_(nullptr) {}

        reference operator*() const { return node_->value; }
        pointer operator->() const { return std::addressof(node_->value); }

        iterator& operator++() {
            if (node_) node_ = node_->next;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const noexcept {
            return node_ == other.node_;
        }

        bool operator!=(const iterator& other) const noexcept {
            return node_ != other.node_;
        }

    private:
        friend class PmrList<T>;
        using Node = typename PmrList<T>::Node;

        Node* node_;
        explicit iterator(Node* node) noexcept : node_(node) {}
    };

    // Итераторы
    iterator begin() noexcept;
    iterator end() noexcept;

private:
    // узел
    struct Node {
        T value;
        Node* prev;
        Node* next;

        template <typename... Args>
        Node(Args&&... args)
            : value(std::forward<Args>(args)...), prev(nullptr), next(nullptr) {}
    };

    allocator_type allocator_;
    Node* head_;
    Node* tail_;
    std::size_t size_;

    Node* create_node(const T& value);
    void destroy_node(Node* node);
};

// реализация шаблона

template <typename T>
PmrList<T>::PmrList(std::pmr::memory_resource* resource)
    : allocator_(resource), head_(nullptr), tail_(nullptr), size_(0) {}

template <typename T>
PmrList<T>::~PmrList() {
    clear();
}

template <typename T>
PmrList<T>::PmrList(PmrList&& other) noexcept
    : allocator_(other.allocator_), head_(other.head_),
      tail_(other.tail_), size_(other.size_) {
    other.head_ = other.tail_ = nullptr;
    other.size_ = 0;
}

template <typename T>
PmrList<T>& PmrList<T>::operator=(PmrList&& other) noexcept {
    if (this != &other) {
        clear();
        allocator_ = other.allocator_;
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    return *this;
}

template <typename T>
typename PmrList<T>::Node* PmrList<T>::create_node(const T& value) {
    std::pmr::polymorphic_allocator<Node> nodeAllocator(allocator_.resource());
    Node* node = std::allocator_traits<decltype(nodeAllocator)>::allocate(nodeAllocator, 1);
    ::new (node) Node(value);
    return node;
}

template <typename T>
void PmrList<T>::destroy_node(Node* node) {
    if (!node) return;
    std::pmr::polymorphic_allocator<Node> nodeAllocator(allocator_.resource());
    node->~Node();
    std::allocator_traits<decltype(nodeAllocator)>::deallocate(nodeAllocator, node, 1);
}

template <typename T>
void PmrList<T>::push_back(const T& value) {
    Node* node = create_node(value);
    node->prev = tail_;
    if (tail_) tail_->next = node;
    tail_ = node;
    if (!head_) head_ = node;
    ++size_;
}

template <typename T>
void PmrList<T>::push_front(const T& value) {
    Node* node = create_node(value);
    node->next = head_;
    if (head_) head_->prev = node;
    head_ = node;
    if (!tail_) tail_ = node;
    ++size_;
}

template <typename T>
void PmrList<T>::pop_back() {
    if (!tail_) return;
    Node* node = tail_;
    tail_ = tail_->prev;
    if (tail_) tail_->next = nullptr;
    else head_ = nullptr;
    destroy_node(node);
    --size_;
}

template <typename T>
void PmrList<T>::pop_front() {
    if (!head_) return;
    Node* node = head_;
    head_ = head_->next;
    if (head_) head_->prev = nullptr;
    else tail_ = nullptr;
    destroy_node(node);
    --size_;
}

template <typename T>
void PmrList<T>::clear() {
    Node* current = head_;
    while (current) {
        Node* next = current->next;
        destroy_node(current);
        current = next;
    }
    head_ = tail_ = nullptr;
    size_ = 0;
}

template <typename T>
std::size_t PmrList<T>::size() const noexcept {
    return size_;
}

template <typename T>
typename PmrList<T>::iterator PmrList<T>::begin() noexcept {
    return iterator(head_);
}

template <typename T>
typename PmrList<T>::iterator PmrList<T>::end() noexcept {
    return iterator(nullptr);
}

#endif
