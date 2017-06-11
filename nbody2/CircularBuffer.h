#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

namespace nbody
{
	template<typename T>
	class CircularBuffer
	{
	public:

		typedef T		  value_type;
		typedef T*		  pointer;
		typedef T const*  const_pointer;
		typedef T&		  reference;
		typedef T const&  const_reference;
		typedef size_t	  size_type;
		typedef ptrdiff_t difference_type;

		explicit CircularBuffer(size_t capacity = 100) :
			buf_(new value_type[capacity]),
			head_(1),
			tail_(0), contents_size_(0),
			buf_size_(capacity)
		{}

		~CircularBuffer()
		{
			delete[] buf_;
		}

		CircularBuffer(CircularBuffer const& src) :
			head_(src.head_), tail_(src.tail_), contents_size_(src.contents_size_), buf_size_(src.buf_size_)
		{
			buf_ = new value_type[buf_size_];

			for (size_t i = 0; i < buf_size_; i++)
			{
				buf_[i] = src.buf_[i];
			}
		}

		CircularBuffer& operator=(CircularBuffer const& src)
		{
			head_ = src.head_;
			tail_ = src.tail_;
			contents_size_ = src.contents_size_;
			buf_size_ = src.buf_size_;
			buf_ = new value_type[buf_size_];

			for (size_t i = 0; i < buf_size_; i++)
			{
				buf_[i] = src.buf_[i];
			}

			return *this;
		}

		reference front()
		{
			return buf_[head_];
		}

		reference back()
		{
			return buf_[tail_];
		}

		const_reference front() const
		{
			return buf_[head_];
		}

		const_reference back() const
		{
			return buf_[tail_];
		}

		reference operator[](size_type index)
		{
			return buf_[(head_ + index) % buf_size_];
		}

		const_reference operator[](size_type index) const
		{
			return buf_[(head_ + index) % buf_size_];
		}

		bool empty() const
		{
			return contents_size_ == 0;
		}

		size_type size() const
		{
			return contents_size_;
		}

		size_type capacity() const
		{
			return buf_size_;
		}

		static size_type max_size()
		{
			return size_type(-1) / sizeof(value_type);
		}

		void push_back(const_reference item)
		{
			increment_tail();
			if (contents_size_ == buf_size_)
				increment_head();
			buf_[tail_] = item;
		}

		void pop_front()
		{
			increment_head();
		}

		void clear()
		{
			tail_ = contents_size_ = 0;
			head_ = 1;
		}

	private:

		void increment_tail()
		{
			++tail_;
			++contents_size_;
			if (tail_ == buf_size_) tail_ = 0;
		}

		void increment_head()
		{
			++head_;
			--contents_size_;
			if (head_ == buf_size_) head_ = 0;
		}

		value_type* buf_;
		size_type head_, tail_, contents_size_, buf_size_;

	};
}

#endif // CIRCULAR_BUFFER_H
