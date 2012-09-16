#pragma once

#include "Rush.h"

namespace Rush
{
	/// @addtogroup util
	/// @{

	template < typename VALUE_TYPE, typename KEY_TYPE >
	class BinaryTree // implements trivial binary search tree
	{

	public:

		struct BSTNode // binary search tree node
		{
			BSTNode(const KEY_TYPE& key, const VALUE_TYPE& val)
				: m_left(0)
				, m_right(0)
				, m_val(val)
				, m_key(key)
			{
			}

			~BSTNode()
			{
				delete m_left;
				delete m_right;
			}
						
			BSTNode* m_left;
			BSTNode* m_right;

			VALUE_TYPE	m_val;
			KEY_TYPE	m_key;
		};

		BinaryTree()
			: m_root(0)
		{}
		~BinaryTree()
		{
			delete m_root;
		}


		VALUE_TYPE* get(const KEY_TYPE& hash)
		{
			BSTNode* tree = m_root;
			while(tree)
			{
				if( hash < tree->m_key ) 
				{
					tree = tree->m_left;
				}
				else if( hash == tree->m_key )
				{
					return &tree->m_val;
				}
				else 
				{
					tree = tree->m_right;
				}
			}			
			return 0;
		}

		INLINE void set(const KEY_TYPE& key, const VALUE_TYPE& val)
		{
			if( m_root == 0 )
			{
				m_root = new BSTNode(key, val);
			}
			else
			{
				BSTNode* tree = m_root;
				while(tree)
				{
					if( key < tree->m_key ) 
					{
						if( tree->m_left == 0 ) 
						{
							tree->m_left = new BSTNode(key, val);
							break;
						}
						else
						{
							tree = tree->m_left;
						}
					}
					else if( key == tree->m_key )	
					{
						tree->m_val = val;
					}
					else 
					{
						if( tree->m_right == 0 ) 
						{
							tree->m_right = new BSTNode(key, val);
							break;
						}
						else
						{
							tree = tree->m_right;
						}
					}
				}
			}
		}

	private:

		BSTNode* m_root;

	};

	/// @}
}
