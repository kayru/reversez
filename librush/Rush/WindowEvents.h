#pragma once

#include "Window.h"
#include "UtilDynamicArray.h"

namespace Rush
{
	namespace WindowEvent
	{
		template <typename T>
		class Listener : public DynamicArray<T>
		{	
		public:
			
			template <typename IDT>
			struct ID{};
			
		public:

			Listener() 
				: owner(NULL)
			{
			}

			Listener(Window* _owner)
				: owner(_owner)
			{
				register_listener();
			}

			Listener(const Listener& rhs)
				: owner(rhs.owner)
			{
				register_listener();
			}

			virtual ~Listener()
			{
				unregister_listener();
			}

			Listener& operator = (const Listener& rhs)
			{
				owner = rhs.owner;
				register_listener();
				DynamicArray<T>::operator =(rhs);
				return *this;
			}

			void set_owner(Window* _owner)
			{
				unregister_listener();
				owner = _owner;
				register_listener();
			}

			static ID<T> tid() { return ID<T>(); } // type ID
					
		private:

			void register_listener();
			void unregister_listener();

		public:
			
			Window*		owner;

		};
		
		///
		
		template <typename EVENT_T>
		class ListenerList : public DynamicArray< Listener<EVENT_T>* >
		{
		public:
			
			template <typename IDT>
			struct ID{};
			
		public:
			virtual ~ListenerList();			
			void broadcast(const EVENT_T& e);
		
			static ID<EVENT_T> tid() { return ID<EVENT_T>(); }
		};
		
		
		///

		struct KeyDown
		{
			KeyDown(){}
			KeyDown(uint8 _code) 
				: code(_code)
			{}
			uint8 code;
		};
		struct KeyUp
		{	
			KeyUp(){}
			KeyUp(uint8 _code) 
				: code(_code)
			{}
			uint8 code;
		};

		struct Resize
		{	
			Resize(){}
			Resize(uint32 _width, uint32 _height)
				: width(_width)
				, height(_height)
			{}
			uint32 width;
			uint32 height;
		};

		struct MouseDown
		{	
			MouseDown():pos(0.0f), button(0), doubleclick(false) {}
			MouseDown(const Vector2& _pos, uint32 _button, bool _doubleclick)
				: pos(_pos)
				, button(_button)
				, doubleclick(_doubleclick)
			{}
			Vector2 pos;
			uint32	button;
			bool	doubleclick;
		};


		struct MouseUp
		{	
			MouseUp(){}
			MouseUp(const Vector2& _pos, uint32 _button)
				: pos(_pos)
				, button(_button)
			{}
			
			Vector2 pos;
			uint32	button;
		};

		struct MouseMove
		{	
			MouseMove(){}
			MouseMove(const Vector2& _pos)
				: pos(_pos)				
			{}
			Vector2 pos;			
		};

		class ListenerCollection
		{				
		public:									
					
			#define DEFINE_LISTENER(TYPE)\
			private: ListenerList<TYPE> m_##TYPE;\
			public: ListenerList<TYPE>& get(ListenerList<TYPE>::ID<TYPE>) { return m_##TYPE; }\
			public: void broadcast(const TYPE & e) { m_##TYPE.broadcast(e); }
			
			DEFINE_LISTENER(KeyDown);
			DEFINE_LISTENER(KeyUp);
			DEFINE_LISTENER(Resize);
			DEFINE_LISTENER(MouseDown);
			DEFINE_LISTENER(MouseUp);
			DEFINE_LISTENER(MouseMove);

			#undef DEFINE_LISTENER
		};
		
		///
		
		template <typename T>
		void Listener<T>::register_listener()
		{
			if( owner )
			{
				ListenerList<T>& list = owner->listeners().get(ListenerList<T>::tid());
				list.push_back(this);
			}
		}
		
		template <typename T>
		void Listener<T>::unregister_listener()
		{
			if( owner )
			{
				ListenerList<T>& list = owner->listeners().get(ListenerList<T>::tid());
				list.find_and_remove(this);
			}
		}
		
		template <typename EVENT_T>
		ListenerList<EVENT_T>::~ListenerList() 
		{ 
			for(uint32 i=0; i<DynamicArray< Listener<EVENT_T>* >::size(); ++i)
			{
				DynamicArray< Listener<EVENT_T>* >::element(i)->owner = NULL;
			}
		}
		
		template <typename EVENT_T>
		void ListenerList<EVENT_T>::broadcast(const EVENT_T& e)
		{
			for(uint32 i=0; i<DynamicArray< Listener<EVENT_T>* >::size(); ++i)
			{
				DynamicArray< Listener<EVENT_T>* >::element(i)->push_back(e);
			}
		}	

		typedef Listener<KeyDown>	KeyDownListener;
		typedef Listener<MouseDown>	MouseDownListener;

	}

}

