#pragma once


class noncopyable
{
public:
    noncopyable (const noncopyable&) = delete;
    noncopyable &operator= (const noncopyable) = delete;
    noncopyable (noncopyable&&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};