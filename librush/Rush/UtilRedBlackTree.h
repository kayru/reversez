#pragma once

#include "Rush.h"

// C++ port from Java: http://www.cs.princeton.edu/~rs/talks/LLRB/Java/RedBlackBST.java

namespace Rush
{
	/// @addtogroup util
	/// @{

	// implements a left-leaning red-black tree
	template < typename VALUE_TYPE, typename KEY_TYPE >
	class RedBlackTree 
	{
		typedef uint8 NODE_COLOUR;
		
		static const NODE_COLOUR NC_BLACK	= 0;
		static const NODE_COLOUR NC_RED		= 1;

	public:

		struct Node // binary search tree node
		{

			Node(const KEY_TYPE& key, const VALUE_TYPE& val, NODE_COLOUR col)
				: m_left(0)
				, m_right(0)		
				, m_val(val)
				, m_key(key)
				, m_col(col)
			{
			}

			~Node()
			{
				delete m_left;
				delete m_right;
			}

			Node* m_left;
			Node* m_right;

			VALUE_TYPE	m_val;
			KEY_TYPE	m_key;

			NODE_COLOUR	m_col;
		};

		RedBlackTree()
			: m_root(0)
		{}
		~RedBlackTree()
		{
			delete m_root;
		}


		INLINE VALUE_TYPE* get(const KEY_TYPE& key)
		{
			Node* tree = m_root;
			while(tree)
			{					
				if( key < tree->m_key ) 
				{
					tree = tree->m_left;
				}
				else if( key == tree->m_key )	
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
			m_root = pr_insert(m_root, key, val);
		}

		/*
		// remove implementation incomplete!
		INLINE void remove(const KEY_TYPE& key)
		{
			m_root = pr_remove(m_root, key);
			m_root->m_col = NC_BLACK;
		}
		*/

	private:

		Node* pr_insert(Node* h, const KEY_TYPE& key, const VALUE_TYPE& val)
		{
			if( h == 0 )
			{
				return new Node(key, val, NC_RED);
			}

			if( pr_is_red(h->m_left) && pr_is_red(h->m_right) )
			{
				pr_flip(h);
			}

			if( key < h->m_key )
			{
				h->m_left = pr_insert(h->m_left, key, val);
			}
			else if( key == h->m_key )
			{
				h->m_val = val;
			}
			else
			{
				h->m_right = pr_insert(h->m_right, key, val);
			}

			if( pr_is_red(h->m_right) )
			{
				h = pr_rotate_left(h);
			}

			if( pr_is_red(h->m_left) && pr_is_red(h->m_left->m_left) )
			{
				h = pr_rotate_right(h);
			}

			return h;
		}

		INLINE Node* pr_rotate_left(Node* h)
		{
			Node* x = h->m_right;

			h->m_right = x->m_left;
			x->m_left = h;
			x->m_col = x->m_left->m_col;
			x->m_left->m_col = NC_RED;

			return x;
		}

		INLINE Node* pr_rotate_right(Node* h)
		{
			Node* x = h->m_left;

			h->m_left = x->m_right;
			x->m_right = h;
			x->m_col = x->m_right->m_col;
			x->m_right->m_col = NC_RED;

			return x;
		}
	
		INLINE void pr_flip(Node* h)
		{
			h->m_col = !h->m_col;

			h->m_left->m_col  = !h->m_left->m_col;
			h->m_right->m_col = !h->m_right->m_col;
		}

		INLINE bool pr_is_red(Node* h)
		{
			return ( h && h->m_col );
		}

		INLINE Node* pr_move_red_left(Node* h)
		{
			pr_flip(h);
			if( pr_is_red(h->m_right->m_left) )
			{
				h->m_right = pr_rotate_right(h->m_right);
				h = pr_rotate_left(h);
				pr_flip(h);
			}
			return h;
		}
		INLINE Node* pr_move_red_right(Node* h)
		{
			pr_flip(h);
			if( pr_is_red(h->m_left->m_left) )
			{
				h = pr_rotate_right(h);
				pr_flip(h);
			}
			return h;			
		}

		INLINE Node* pr_fix_up(Node* h)
		{
			if( pr_is_red(h->m_right) )
			{
				h = pr_rotate_left(h);
			}
			if( pr_is_red(h->m_left) && pr_is_red(h->m_left->m_left) )
			{
				h = pr_rotate_right(h);
			}
			if( pr_is_red(h->m_left) && pr_is_red(h->m_right) )
			{
				pr_flip(h);
			}

			return h;
		}

		Node* pr_remove_min(Node* h)
		{
			if( h->m_left == 0 ) 
			{
				delete h;
				return 0;
			}

			if( !pr_is_red(h->m_left) && !pr_is_red(h->m_left->m_left) )
			{
				h = pr_move_red_left(h);
			}
			h->m_left = pr_remove_min(h->m_left);
			
			return pr_fix_up(h);
		}

		INLINE Node* pr_find_min(Node* h)
		{
			while(h->m_left)
			{				
				h = h->m_left;
			}			
			return h;
		}

		Node* pr_remove(Node* h, const KEY_TYPE& key)
		{
			if( key < h->m_key )
			{
				if( !pr_is_red(h->m_left) && !pr_is_red(h->m_left->m_left) )
				{
					h = pr_move_red_left(h);
					h->m_left = pr_remove(h->m_left, key);
				}
			}
			else
			{
				if( pr_is_red(h->m_left) )
				{
					h = pr_rotate_right(h);
				}
				if( key == h->m_key && h->m_right == 0 )
				{
					return 0;
				}
				if( !pr_is_red(h->m_right) && !pr_is_red(h->m_right->m_left) )
				{
					h = pr_move_red_right(h);
				}
				if( key == h->m_key )
				{
					Node* min_node = pr_find_min(h->m_right);
					h->m_key = min_node->m_key;
					h->m_val = min_node->m_val;
					h->m_right = pr_remove_min(h->m_right);
				}
				else h->m_right = pr_remove(h->m_right, key);
			}
			return pr_fix_up(h);
		}
		
		Node* m_root;
	};

	/// @}
}
