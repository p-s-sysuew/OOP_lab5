#include <gtest/gtest.h>
#include "../include/point.h"
#include "../include/trapezoid.h"
#include "../include/rhombus.h"
#include "../include/ pentagon.h"
#include "../include/array.h"
#include <memory>
#include <cmath>

// Тесты для класса Point
TEST(PointTest, DefaultConstructor) {
    Point<int> p;
    EXPECT_EQ(p.x(), 0);
    EXPECT_EQ(p.y(), 0);
}

TEST(PointTest, ParameterizedConstructor) {
    Point<double> p(3.5, 2.7);
    EXPECT_DOUBLE_EQ(p.x(), 3.5);
    EXPECT_DOUBLE_EQ(p.y(), 2.7);
}

TEST(PointTest, EqualityOperator) {
    Point<int> p1(1, 2);
    Point<int> p2(1, 2);
    Point<int> p3(3, 4);
    
    EXPECT_TRUE(p1 == p2);
    EXPECT_FALSE(p1 == p3);
}

TEST(PointTest, SetMethods) {
    Point<int> p;
    p.set_x(10);
    p.set_y(20);
    
    EXPECT_EQ(p.x(), 10);
    EXPECT_EQ(p.y(), 20);
}

// Тесты для класса Trapezoid
TEST(TrapezoidTest, ConstructorAndArea) {
    Trapezoid<int> trap(0, 0, 4, 0, 1, 3, 3, 3);
    
    EXPECT_NO_THROW(trap.area());
    EXPECT_GT(trap.area(), 0);
}

TEST(TrapezoidTest, GeometricCenter) {
    Trapezoid<int> trap(0, 0, 4, 0, 1, 3, 3, 3);
    Point<int> center = trap.geometric_center();
    
    EXPECT_EQ(center.x(), 2);
    EXPECT_EQ(center.y(), 1);
}

TEST(TrapezoidTest, CopyConstructor) {
    Trapezoid<int> trap1(0, 0, 4, 0, 1, 3, 3, 3);
    Trapezoid<int> trap2(trap1);
    
    EXPECT_TRUE(trap1 == trap2);
}

// Тесты для класса Rhombus
TEST(RhombusTest, ConstructorAndArea) {
    Rhombus<int> rhomb(0, 0, 6, 4);
    
    EXPECT_DOUBLE_EQ(rhomb.area(), 12.0);
    EXPECT_NO_THROW(rhomb.geometric_center());
}

TEST(RhombusTest, GeometricCenter) {
    Rhombus<double> rhomb(2.5, 3.5, 6, 4);
    Point<double> center = rhomb.geometric_center();
    
    EXPECT_DOUBLE_EQ(center.x(), 2.5);
    EXPECT_DOUBLE_EQ(center.y(), 3.5);
}

TEST(RhombusTest, EqualityOperator) {
    Rhombus<int> rhomb1(0, 0, 6, 4);
    Rhombus<int> rhomb2(0, 0, 6, 4);
    Rhombus<int> rhomb3(1, 1, 5, 3);
    
    EXPECT_TRUE(rhomb1 == rhomb2);
    EXPECT_FALSE(rhomb1 == rhomb3);
}

// Тесты для класса Pentagon
TEST(PentagonTest, ConstructorAndArea) {
    Pentagon<double> pent(0, 0, 5.0);
    
    EXPECT_GT(pent.area(), 0);
    EXPECT_NO_THROW(pent.geometric_center());
}

TEST(PentagonTest, GeometricCenter) {
    Pentagon<float> pent(1.0f, 2.0f, 3.0f);
    Point<float> center = pent.geometric_center();
    
    EXPECT_FLOAT_EQ(center.x(), 1.0f);
    EXPECT_FLOAT_EQ(center.y(), 2.0f);
}

// Тесты для класса Array
TEST(ArrayTest, DefaultConstructor) {
    Array<int> arr;
    
    EXPECT_EQ(arr.size(), 0);
    EXPECT_GE(arr.capacity(), 10);
}

TEST(ArrayTest, PushBackAndAccess) {
    Array<std::string> arr;
    arr.push_back("hello");
    arr.push_back("world");
    
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], "hello");
    EXPECT_EQ(arr[1], "world");
}

TEST(ArrayTest, RemoveElement) {
    Array<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    
    arr.remove(1); // Удаляем элемент с индексом 1 (значение 2)
    
    EXPECT_EQ(arr.size(), 2);
    EXPECT_EQ(arr[0], 1);
    EXPECT_EQ(arr[1], 3);
}

TEST(ArrayTest, CopyConstructor) {
    Array<int> arr1;
    arr1.push_back(1);
    arr1.push_back(2);
    
    Array<int> arr2(arr1);
    
    EXPECT_EQ(arr2.size(), 2);
    EXPECT_EQ(arr2[0], 1);
    EXPECT_EQ(arr2[1], 2);
}

// Тесты для полиморфизма фигур
TEST(FigurePolymorphismTest, GeometricCenter) {
    std::shared_ptr<Figure<int>> trap = std::make_shared<Trapezoid<int>>(0, 0, 4, 0, 1, 3, 3, 3);
    std::shared_ptr<Figure<int>> rhomb = std::make_shared<Rhombus<int>>(0, 0, 6, 4);
    std::shared_ptr<Figure<int>> pent = std::make_shared<Pentagon<int>>(0, 0, 5);
    
    EXPECT_NO_THROW(trap->geometric_center());
    EXPECT_NO_THROW(rhomb->geometric_center());
    EXPECT_NO_THROW(pent->geometric_center());
}

TEST(FigurePolymorphismTest, AreaCalculation) {
    std::shared_ptr<Figure<int>> trap = std::make_shared<Trapezoid<int>>(0, 0, 4, 0, 1, 3, 3, 3);
    std::shared_ptr<Figure<int>> rhomb = std::make_shared<Rhombus<int>>(0, 0, 6, 4);
    
    EXPECT_GT(trap->area(), 0);
    EXPECT_GT(rhomb->area(), 0);
}

TEST(FigurePolymorphismTest, TypeConversion) {
    std::shared_ptr<Figure<int>> trap = std::make_shared<Trapezoid<int>>(0, 0, 4, 0, 1, 3, 3, 3);
    double area = static_cast<double>(*trap);
    
    EXPECT_GT(area, 0);
}

// Тесты для массива фигур
TEST(FigureArrayTest, ArrayOfFigures) {
    Array<std::shared_ptr<Figure<int>>> figures;
    
    figures.push_back(std::make_shared<Trapezoid<int>>(0, 0, 4, 0, 1, 3, 3, 3));
    figures.push_back(std::make_shared<Rhombus<int>>(0, 0, 6, 4));
    figures.push_back(std::make_shared<Pentagon<int>>(0, 0, 5));
    
    EXPECT_EQ(figures.size(), 3);
    
    double total = 0;
    for (size_t i = 0; i < figures.size(); ++i) {
        total += figures[i]->area();
    }
    
    EXPECT_GT(total, 0);
}

TEST(FigureArrayTest, CloneOperation) {
    auto original = std::make_shared<Trapezoid<int>>(0, 0, 4, 0, 1, 3, 3, 3);
    auto cloned = original->clone();
    
    EXPECT_TRUE(*original == *cloned);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}