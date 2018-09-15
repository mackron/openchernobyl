// Copyright (C) 2018 David Reid. See included LICENSE file.

// Rectangles are top to bottom, left to right.

template <typename T = ocInt32>
struct ocRect
{
    T left;
    T top;
    T right;
    T bottom;

    ocRect(T _left, T _top, T _right, T _bottom)
        : left(_left), top(_top), right(_right), bottom(_bottom)
    {
    }
};

template <typename T> ocRect<T> ocRectInit(T left, T top, T right, T bottom)
{
    return ocRect<T>(left, top, right, bottom);
}

template <typename T> ocRect<T> ocRectInitInsideOut()
{
    return ocRect<T>(
        ocTypeMax<T>(),
        ocTypeMin<T>(),
        ocTypeMax<T>(),
        ocTypeMin<T>()
    );
}

typedef ocRect<ocInt32>   ocRectI;
typedef ocRect<ocFloat32> ocRectF;
