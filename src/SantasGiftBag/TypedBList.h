//*** LICENSE ***
//ColumnListView, its associated classes and source code, and the other components of Santa's Gift Bag are
//being made publicly available and free to use in freeware and shareware products with a price under $25
//(I believe that shareware should be cheap). For overpriced shareware (hehehe) or commercial products,
//please contact me to negotiate a fee for use. After all, I did work hard on this class and invested a lot
//of time into it. That being said, DON'T WORRY I don't want much. It totally depends on the sort of project
//you're working on and how much you expect to make off it. If someone makes money off my work, I'd like to
//get at least a little something.  If any of the components of Santa's Gift Bag are is used in a shareware
//or commercial product, I get a free copy.  The source is made available so that you can improve and extend
//it as you need. In general it is best to customize your ColumnListView through inheritance, so that you
//can take advantage of enhancements and bug fixes as they become available. Feel free to distribute the 
//ColumnListView source, including modified versions, but keep this documentation and license with it.

//Conventions:
//    Global constants (declared with const) and #defines - all uppercase letters with words separated 
//        by underscores.
//        (E.G., #define MY_DEFINE 5).
//        (E.G., const int MY_CONSTANT = 5;).
//    New data types (classes, structs, typedefs, etc.) - begin with an uppercase letter followed by
//        lowercase words separated by uppercase letters.  Enumerated constants contain a prefix
//        associating them with a particular enumerated set.
//        (E.G., typedef int MyTypedef;).
//        (E.G., enum MyEnumConst {MEC_ONE, MEC_TWO};)
//    Private member variables - begin with "m_" followed by lowercase words separated by underscores.
//        (E.G., int m_my_private_member;).
//    Public or protected member variables - begin with a lowercase letter followed by lowercase words
//        separated by underscores.
//        (E.G., int my_public_member;).
//    Arguments - begin with a lowercase letter followed by lowercase words separated by underscores,
//        preceded by a_ if there is ambiguity with a similarly named public or protected member variable.
//        (E.G., int my_argument; int a_my_member;).
//    Local variables - begin with a lowercase letter followed by
//        lowercase words separated by underscores.
//        (E.G., int my_local;).
//    Functions (member or global) - begin with an uppercase letter followed by lowercase words
//        separated by uppercase letters.
//        (E.G., void MyFunction();).
//    Global variables - begin with "g_" followed by lowercase words separated by underscores.
//        (E.G., int g_my_global;).

//Usage:
//class CharPtrList : public TypedBList<char*>{};

#ifndef __TYPED_LIST_TEMPLATE_H_
#define __TYPED_LIST_TEMPLATE_H_

#include <List.h>

template<class list_content_type> class TypedBList
{
	private:
		BList m_list;

	public:
		TypedBList(int32 itemsPerBlock = 20)
		: m_list(itemsPerBlock)
		{ }

		TypedBList(const TypedBList<list_content_type>& copy)
		: m_list(copy.m_list)
		{ }

		TypedBList &operator=(const TypedBList<list_content_type>& from)
		{
			m_list = from.m_list;
		}
		
		bool AddItem(list_content_type item)									{return m_list.AddItem((void*)item);}
		bool AddItem(list_content_type item, int32 atIndex)						{return m_list.AddItem((void*)item,atIndex);}
		bool AddList(TypedBList<list_content_type>* newItems)					{return m_list.AddList(&newItems->m_list);}
		bool AddList(TypedBList<list_content_type>* newItems, int32 atIndex)	{return m_list.AddList(&newItems->m_list,atIndex);}
		bool RemoveItem(list_content_type item)									{return m_list.RemoveItem((void*)item);}
		list_content_type RemoveItem(int32 index)								{return (list_content_type)m_list.RemoveItem(index);}
		bool RemoveItems(int32 index, int32 count)								{return m_list.RemoveItems(index,count);}
		bool ReplaceItem(int32 index, list_content_type newItem)				{return m_list.ReplaceItem(index,(void*)newItem);}
		void MakeEmpty()														{m_list.MakeEmpty();}

		void SortItems(int (*cmp)(list_content_type, list_content_type))		{m_list.SortItems((int (*)(const void *, const void *))cmp);}
		bool SwapItems(int32 indexA, int32 indexB)								{return m_list.SwapItems(indexA,indexB);}
		bool MoveItem(int32 fromIndex, int32 toIndex)							{return m_list.MoveItem(fromIndex,toIndex);}

		list_content_type ItemAt(int32 index) const								{return (list_content_type)m_list.ItemAt(index);}
		list_content_type ItemAtFast(int32 index) const							{return (list_content_type)m_list.ItemAtFast(index);}
		list_content_type FirstItem() const										{return (list_content_type)m_list.FirstItem();}
		list_content_type LastItem() const										{return (list_content_type)m_list.LastItem();}
		list_content_type* Items() const										{return (list_content_type*)m_list.Items();}

		bool HasItem(list_content_type item) const								{return m_list.HasItem((void*)item);}
		int32 IndexOf(list_content_type item) const								{return m_list.IndexOf((void*)item);}
		int32 CountItems() const												{return m_list.CountItems();}
		bool IsEmpty() const													{return m_list.IsEmpty();}

		void DoForEach(bool (*func)(list_content_type))							{m_list.DoForEach((bool (*)(void *))func);}
		void DoForEach(bool (*func)(list_content_type, void *), void *)			{m_list.DoForEach((bool (*)(void *, void *))func);}
};


#endif //__TYPED_LIST_TEMPLATE_H_

