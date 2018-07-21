class UInt24
{
public:
	UInt24()
		: m_value(0u)
	{
	}
	UInt24(const UInt24 &other)
		: m_value(other.m_value)
	{
	}
	UInt24(const auto &other)
		: m_value(mask(other))
	{
	}

	UInt24 & operator=(const UInt24 &other)
	{
		m_value = other.m_value;
		return *this;
	}

	UInt24 & operator=(const auto &other)
	{
		m_value = mask(other);
		return *this;
	}

	operator unsigned int() const
	{
		return m_value;
	}

	UInt24 operator++() // prefix-inc
	{
		m_value = mask(m_value + 1u);
		return *this;
	}

	UInt24 operator++(int) // postfix-inc
	{
		UInt24 old(*this);
		++(*this);
		return old;
	}

	UInt24 operator--() // prefix-dec
	{
		m_value = mask(m_value - 1u);
		return *this;
	}

	UInt24 operator--(int) // postfix-dec
	{
		UInt24 old(*this);
		--(*this);
		return old;
	}

	UInt24 & operator+=(const UInt24 &other)
	{
		m_value = mask(m_value + other.m_value);
		return *this;
	}

	UInt24 & operator+=(const auto &other)
	{
		m_value = mask(m_value + mask(other));
		return *this;
	}

	UInt24 & operator-=(const UInt24 &other)
	{
		m_value = mask(m_value - other.m_value);
		return *this;
	}

	UInt24 & operator-=(const auto &other)
	{
		m_value = mask(m_value - mask(other));
		return *this;
	}

	UInt24 operator&(const UInt24 &other) const
	{
		return UInt24(m_value & other.m_value);
	}

	UInt24 operator&(const auto &other) const
	{
		return UInt24(m_value & mask(other));
	}

	UInt24 operator|(const UInt24 &other) const
	{
		return UInt24(m_value | other.m_value);
	}

	UInt24 operator|(const auto &other) const
	{
		return UInt24(m_value | mask(other));
	}

	UInt24 operator^(const UInt24 &other) const
	{
		return UInt24(m_value ^ other.m_value);
	}

	UInt24 operator^(const auto &other) const
	{
		return UInt24(m_value ^ mask(other));
	}

	const UInt24 operator+(const UInt24 &other) const
	{
		return UInt24(m_value + other.m_value);
	}

	const UInt24 operator+(const auto &other) const
	{
		return UInt24(m_value + mask(other));
	}

	const UInt24 operator-(const UInt24 &other) const
	{
		return UInt24(m_value - other.m_value);
	}

	const UInt24 operator-(const auto &other) const
	{
		return UInt24(m_value - mask(other));
	}

	const UInt24 operator*(const UInt24 &other) const
	{
		return UInt24(m_value * other.m_value);
	}

	const UInt24 operator*(const auto &other) const
	{
		return UInt24(m_value * mask(other));
	}

	const UInt24 operator/(const UInt24 &other) const
	{
		return UInt24(m_value / other.m_value);
	}

	const UInt24 operator/(const auto &other) const
	{
		return UInt24(m_value / mask(other));
	}

	bool operator<(const UInt24 &other) const
	{
		return m_value < other.m_value;
	}

	bool operator<(const auto &other) const
	{
		return m_value < mask(other);
	}

	bool operator==(const UInt24 &other) const
	{
		return m_value == other.m_value;
	}

	bool operator==(const auto &other) const
	{
		return m_value == mask(other);
	}

	bool operator!=(const UInt24 &other) const
	{
		return m_value != other.m_value;
	}

	bool operator!=(const auto &other) const
	{
		return m_value != mask(other);
	}

	bool operator>(const UInt24 &other) const
	{
		return m_value > other.m_value;
	}

	bool operator>(const auto &other) const
	{
		return m_value > mask(other);
	}

protected:
	unsigned int mask(const auto &value) const
	{
		return static_cast<unsigned int>(value) & 0x00FFFFFFu;
	}

protected:
	unsigned int m_value;
};


class Int24
{
public:
	Int24()
		: m_value(0)
	{
	}
	Int24(const Int24 &other)
		: m_value(other.m_value)
	{
	}
	Int24(const auto &other)
		: m_value(extend(other))
	{
	}

	Int24 & operator=(const Int24 &other)
	{
		m_value = other.m_value;
		return *this;
	}

	Int24 & operator=(const auto &other)
	{
		m_value = extend(other);
		return *this;
	}

	operator int() const
	{
		return m_value;
	}

	Int24 operator++() // prefix-inc
	{
		m_value = extend(m_value + 1);
		return *this;
	}

	Int24 operator++(int) // postfix-inc
	{
		Int24 old(*this);
		++(*this);
		return old;
	}

	Int24 operator--() // prefix-dec
	{
		m_value = extend(m_value - 1);
		return *this;
	}

	Int24 operator--(int) // postfix-dec
	{
		Int24 old(*this);
		--(*this);
		return old;
	}

	Int24 & operator+=(const Int24 &other)
	{
		m_value = extend(m_value + other.m_value);
		return *this;
	}

	Int24 & operator+=(const auto &other)
	{
		m_value = extend(m_value + extend(other));
		return *this;
	}

	Int24 & operator-=(const Int24 &other)
	{
		m_value = extend(m_value - other.m_value);
		return *this;
	}

	Int24 & operator-=(const auto &other)
	{
		m_value = extend(m_value - extend(other));
		return *this;
	}

	Int24 operator&(const Int24 &other) const
	{
		return Int24(m_value & other.m_value);
	}

	Int24 operator&(const auto &other) const
	{
		return Int24(m_value & extend(other));
	}

	Int24 operator|(const Int24 &other) const
	{
		return Int24(m_value | other.m_value);
	}

	Int24 operator|(const auto &other) const
	{
		return Int24(m_value | extend(other));
	}

	Int24 operator^(const Int24 &other) const
	{
		return Int24(m_value ^ other.m_value);
	}

	Int24 operator^(const auto &other) const
	{
		return Int24(m_value ^ extend(other));
	}

	const Int24 operator+(const Int24 &other) const
	{
		return Int24(m_value + other.m_value);
	}

	const Int24 operator+(const auto &other) const
	{
		return Int24(m_value + extend(other));
	}

	const Int24 operator-(const Int24 &other) const
	{
		return Int24(m_value - other.m_value);
	}

	const Int24 operator-(const auto &other) const
	{
		return Int24(m_value - extend(other));
	}

	const Int24 operator*(const Int24 &other) const
	{
		return Int24(m_value * other.m_value);
	}

	const Int24 operator*(const auto &other) const
	{
		return Int24(m_value * extend(other));
	}

	const Int24 operator/(const Int24 &other) const
	{
		return Int24(m_value / other.m_value);
	}

	const Int24 operator/(const auto &other) const
	{
		return Int24(m_value / extend(other));
	}

	bool operator<(const Int24 &other) const
	{
		return m_value < other.m_value;
	}

	bool operator<(const auto &other) const
	{
		return m_value < extend(other);
	}

	bool operator==(const Int24 &other) const
	{
		return m_value == other.m_value;
	}

	bool operator==(const auto &other) const
	{
		return m_value == extend(other);
	}

	bool operator!=(const Int24 &other) const
	{
		return m_value != other.m_value;
	}

	bool operator!=(const auto &other) const
	{
		return m_value != extend(other);
	}

	bool operator>(const Int24 &other) const
	{
		return m_value > other.m_value;
	}

	bool operator>(const auto &other) const
	{
		return m_value > extend(other);
	}

protected:
	int extend(const auto &value) const
	{
		unsigned int v = static_cast<unsigned int>(value) & 0x00FFFFFFu;
		if (v & (1u << 23))
			v |= 0xFF000000u;
		return static_cast<int>(v);
	}

protected:
	int m_value;
};
