#ifndef _HACHE_INC
#define _HACHE_INC

#define TMAXCLEFHACHAGE 32

class CBaseHacheValue
{
public:
	char Key[TMAXCLEFHACHAGE];
	void Detruire(){};
};

struct IndexHachage
{
	size_t i;
	unsigned int hache;
};

template <class T>
class THachage
{
	size_t l,a;
	T * v;				
	IndexHachage * Index; //Index vers les clefs triées

	int ObtId(const char * clef)const;
	int ObtId(const char * clef, size_t lclef)const;
public:
	~THachage()
	{
		if (Index)
			free(Index);
		for (size_t i=0; i<l; i++)
			v[i].Detruire();
		if (v)
			free(v);
	}
	THachage()
	{
		Index=NULL;
		v=NULL;
		l=a=0;
	}
	bool Existe(const char * clef)const
	{
		return (ObtId(clef)>=0);
	}
/*	T & operator[](const char * clef) const
	{
		return v[ObtId(clef)];
	}*/
	T * operator[](const char * clef) const
	{
		int i=ObtId(clef);
		if (i>=0)
			return &v[i];
		else
			return NULL;
	}
	T * operator[](int i)const
	{
		return &v[i];
	}

	T & Val(const char *clef, size_t lclef)
	{
		return v[ObtId(clef,lclef)];
	}
	T* Ajouter(const char * clef);
	T* Ajouter(const char * clef,size_t lclef);
};

inline unsigned int CalculerHache(const char *clef)
{
	unsigned int i=0;
	unsigned int hache=0;
	while (clef[i] && i<4)
	{
		hache<<=8;
		hache+=clef[i++];
	}
	hache<<=((4-i)*8);
	return hache;
}

template<class T>
int THachage<T>::ObtId(const char * clef) const
{
	if (l==0)
		return -1;

	int hache=CalculerHache(clef);
	
	int inf=-1;
	int sup=int(l);
	IndexHachage * idx=Index;
	
	while (inf<sup-1)
	{
		int mil=(inf+sup)/2;
		int h=idx[mil].hache;
		int ret=(hache<h?-1:(hache>h?1:0));
		if (ret==0)
			ret=strcmp(clef,v[idx[mil].i].Key);
		if (ret<0)
			sup=mil;
		else if (ret>0)
			inf=mil;
		else
			return int(idx[mil].i);
	}
	return -sup-1;//Où est-ce qu'il faut insérer
}

template<class T>
T * THachage<T>::Ajouter(const char * clef)
{
	int ret=ObtId(clef);
	if (ret<0)
	{
		int OuInserer=-ret-1;
		if (l==a)
		{
			a+=128;
			v=(T*) realloc(v,a*sizeof(T));
			Index=(IndexHachage*)realloc(Index,a*sizeof(IndexHachage));
		}
		if (OuInserer!=l)
		{
			memmove(&Index[OuInserer+1],&Index[OuInserer],
				(l-OuInserer)*sizeof(IndexHachage));
		}
		Index[OuInserer].i=l;
		Index[OuInserer].hache=CalculerHache(clef);

		strncpy(v[l].Key,clef,TMAXCLEFHACHAGE);
		ret=int(l++);
	}
	return &v[ret];
}

#endif
