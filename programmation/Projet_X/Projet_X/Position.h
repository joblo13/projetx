struct Position
{
	Position();

	float x; 
	float y; 
	float z;

	bool m_xDirection;
	bool m_yDirection;
	bool m_zDirection;

	void setDirection(bool i_xDirection, bool i_yDirection, bool i_zDirection)
	{	
		m_xDirection = i_xDirection;
		m_yDirection = i_yDirection;
		m_zDirection = i_zDirection;
	}

	bool operator<(const Position& i_position)const
	{
		if(x == i_position.x)
		{
			if(y == i_position.y)
			{
				if(m_zDirection)
					return z < i_position.z;
				else
					return z > i_position.z;
			}
			else
			{
				if(m_yDirection)
					return y < i_position.y;
				else
					return y > i_position.y;
			}
		}
		else
		{
			if(m_xDirection)
				return x < i_position.x;
			else
				return x > i_position.x;
		}
	}
	operator size_t () const 
	{ 
		return (size_t)(x * 100) ^ (size_t)((short)(y * 100)<<2) ^ (size_t)((short)(z * 100)<<4); 
	}
};