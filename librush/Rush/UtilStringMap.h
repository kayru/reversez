#pragma once

#include <malloc.h> // for malloc/free
#include <vector>	// for the InOrder function.

/*
This code is described in “Ternary Search Trees” by Jon
Bentley and Robert Sedgewick in the April, 1998, Dr. Dobb’s Journal.

http://www.ddj.com/windows/184410528

Other algorithms and analysis are available here:

http://www.cs.princeton.edu/~rs/strings/

Ternary search trees offer substantial advantages over both
binary search trees and digital search tries. We feel that they are superior
to hashing in many applications for the following reasons:

Efficient and easy to implement
No extra overhead for insertion or successful searches
Usually substantially faster than hashing for unsuccessful searches
Gracefully grow and shrink; hash tables need to be rebuilt after large size changes
Support advanced searches, such as partial-match and near-neighbor search.
Traversal to report items in sorted order. 

————————————————-
http://meshula.net/wordpress/?p=183
I’ve added the StringMap template wrapper to encapsulate the glboal variables
in Bentley & Sedgewick’s original code. Otherwise, this implementation is
substantially identical.

StringMap needs a simple and efficient iterator in order to get rid
of the InOrder method

I haven’t bothered with adding an erase function, or STL compatibility,
because my goal is an as-light-as-possible symbol table look up. The extra
complexity doesn’t help that goal.

*/

#  include <assert.h>
#  define STRINGMAP_ERROR assert(false);

namespace Rush
{

	/// @addtogroup util
	/// @{

	template <class Data>
	class StringMap
	{
	public:
		static const int buffPoolGrowSize = 1000;
		static const int maxPools = 1000;

		struct Tnode {
			char	splitchar;
			Data	data;
			Tnode*	lokid;
			Tnode*	eqkid;	// contains middle kid if splitchar != 0, else char* pointer to full string (node is a leaf)
			Tnode*	hikid;
		};

		StringMap() : buf(0), bufn(0), freen(0), size(0), root(0)
		{
			memset(freearr, 0, sizeof(freearr));
		}

		~StringMap()
		{
			for (int i = 0; i < freen; i++)
				delete[](freearr[i]);
		}

		// A major cost of insert functions is calling malloc to create each node.
		// This function uses the ancient C technique of allocating a buffer of nodes and
		// dealing them out as needed. Profiling shows that this eliminates most of the
		// time spent in storage allocation. 

		void insert(char const* s, const Data& data)
		{
			char const* instr = s;
			Tnode* pp;
			Tnode** p;
			p = &root;
			while ( (pp = *p)!=0 )
			{
				// optimization: save a difference in a comparison
				int d;
				if ((d = *s - pp->splitchar) == 0)
				{
					if (*s++ == 0)
						return;
					p = &(pp->eqkid);
				}
				else if (d < 0)
					p = &(pp->lokid);
				else
					p = &(pp->hikid);
			}
			for (;;)
			{
				// *p = (Tptr) malloc(sizeof(Tnode));	reference code; this is what the pools replace
				if (bufn-- == 0)
				{
					if (freen == maxPools - 1)
					{
						STRINGMAP_ERROR
					}
					buf = new Tnode[buffPoolGrowSize];
					freearr[freen++] = buf;
					bufn = buffPoolGrowSize - 1;
				}
				*p = buf++;
				pp = *p;
				pp->splitchar = *s;
				pp->lokid = pp->eqkid = pp->hikid = 0;
				if (*s++ == 0)
				{
					// store a pointer to every string in the tree; this data will be used by later search
					// exploit the fact that a node with a null splitchar cannot have an eqkid
					pp->eqkid = (Tnode*) instr;
					pp->data = data;
					++size;	// count it
					return;
				}
				p = &(pp->eqkid);
			}
		}

		// If the search character is less, go to lokid;
		// if it is greater, go to hikid;
		// if it is equal, go to the next character and eqkid. 

		bool find(char const* s, Data* data) const
		{
			Tnode* p;
			p = root;
			while (p)
			{
				if (*s < p->splitchar)
					p = p->lokid;
				else if (*s == p->splitchar)
				{
					if (*s++ == 0)
					{
						*data = p->data;
						return true;
					}

					p = p->eqkid;
				}
				else
					p = p->hikid;
			}
			return false;
		}

		void inOrderR(std::vector<std::pair<char const*, Data*> >& nodes, Tnode* p) const
		{
			if (!p)
				return;

			inOrderR(nodes, p->lokid);

			if (p->splitchar)
				inOrderR(nodes, p->eqkid);
			else
				nodes.push_back(std::pair<char const*, Data*>((char const*)p->eqkid, &p->data));

			inOrderR(nodes, p->hikid);
		}

		void inOrder(std::vector<std::pair<char const*, Data*> >& nodes) const
		{
			inOrderR(nodes, root);
		}

		// print all members of the tree in order

		void traverse(Tnode* p)
		{
			if (!p)
				return;
			traverse(p->lokid);
			if (p->splitchar)
				traverse(p->eqkid);
			else
				printf("%s\n", (char *) p->eqkid);
			traverse(p->hikid);
		}

		Tnode* buf;					// next available Tnode
		int bufn;					// number of Tnodes available in current pool
		int freen;					// number of pools to be freed
		Tnode* freearr[maxPools];	// all the allocated pools
		int size;					// number of nodes

		Tnode* root;				// root of the tree
	};

	/// @}

}

