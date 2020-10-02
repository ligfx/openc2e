#pragma once

template <typename T>
class enumerate_impl
{
public:
    struct value_type
    {
        size_t i;
        typename T::value_type & value;
        typename T::value_type * operator->() {
          return &value;
        }
        operator typename T::value_type () {
          return value;
        }
    };

    struct iterator
    {
        iterator(typename T::iterator it_, size_t i_=0) :
            it(it_), i(i_)
        {}

        iterator operator++()
        {
            return iterator(++it, ++i);
        }

        bool operator!=(iterator other)
        {
            return it != other.it;
        }

        value_type operator*()
        {
            return value_type{i, *it};
        }

        typename T::iterator it;
        size_t i;
    };

    enumerate_impl(T & t) : container(t) {}

    iterator begin()
    {
        return iterator(container.begin());
    }

    iterator end()
    {
        return iterator(container.end());
    }

private:
    T & container;
};

template <typename T>
enumerate_impl<T> enumerate(T & t)
{
    return enumerate_impl<T>(t);
}
