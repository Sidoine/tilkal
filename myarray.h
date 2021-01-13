inline int IDfromUID(int64 UID)
{
	return int(UID & 0xFFFFFFFF);	
}

inline void IncHighPartfromUID(int64 & UID)
{
	UID=((UID>>32)+1)<<32;
}

class CMyArrayMember
{
public:
	union
	{
		int _Free;
		int Id;
	};
	int64 UID;
	void Destroy(){}
	inline void NewUID()
	{
		IncHighPartfromUID(UID);
		UID+=Id;
	}	
};


template<class T, int Size>
class TMyArray
{
public:
	T *t;
	int Free,Length,Allocated;
	
	int New()
	{
		if (Free==Length)
		{
			if (Length==Allocated)
			{
				Allocated+=Size;
				t=(T*)realloc(t,Allocated*sizeof(T));
			}
			Length++;
			t[Free].Id=Free;
			IncHighPartfromUID(t[Free].UID);
			t[Free].UID+=Free;
			return Free++;
		}
		else
		{
			int tmp=Free;
			Free=t[Free]._Free;
			t[tmp].Id=tmp;
			IncHighPartfromUID(t[Free].UID);
			t[Free].UID+=Free;
			return tmp;
		}
	}
	
	void Remove(int i)
	{
		t[i].Destroy();
		t[i]._Free=Free;
		Free=i;		
	}
	
	T & operator[](int i)
	{
		return t[i];
	}
	
	TMyArray()
	{
		Free=Length=0;
		t=NULL;
	}

	bool IsActive(int i)
	{
		return i<Length && t[i].Id==i;
	}
	
	bool SaveAll(const char * filename)
	{
		FILE *fichier=fopen(filename,"wb");
		if (fichier == NULL)
			return false;
		fwrite(&Length,sizeof(size_t),1,fichier);
		fwrite(&Free,sizeof(size_t),1,fichier);  
		fwrite(t,sizeof(T),Length,fichier);
		fclose(fichier);	
		return true;
	}
	
	bool LoadAll(const char * filename)
	{
		FILE *fichier=fopen(filename,"rb");
		if (fichier == NULL)
			return false;
		fread(&Length,sizeof(size_t),1,fichier);
		fread(&Free,sizeof(size_t),1,fichier); 
		Allocated=Length;
		t=(T*)malloc(Allocated*sizeof(T)); 
		fread(t,sizeof(T),Length,fichier);
		fclose(fichier);	
		return true;
	}
	
	bool IsAlive(int64 UID)
	{
		int o=IDfromUID(UID);
		return (IsActive(o) && t[o].UID==UID);
	}
};

template<class T, int Size=16>
class AutoArray
{
	T *t;
	int Allocated;
public:
	int Length;
	AutoArray()
	{
		t=NULL;
		Length=Allocated=0;
	}
	void Add()
	{
		if (Length==Allocated)
		{
			Allocated+=Size;
			t=(T*)realloc(t,sizeof(T)*Allocated);
		}
		Length++;
	}
		
	void Add(T & i)
	{
		if (Length==Allocated)
		{
			Allocated+=Size;
			t=(T*)realloc(t,sizeof(T)*Allocated);
		}
		t[Length++]=i;
	}

	T & Last()
	{
		return t[Length-1];
	}
	T & operator[](int i)
	{
		return t[i];
	}
	void operator=(AutoArray<T,Size> & b)
	{
		Empty();
		t=b.t;
		Length=b.Length;
		Allocated=b.Allocated;
		b.t=NULL;
		b.Length=b.Allocated=0;
	}
	void Empty()
	{
		if (t)
		{
			free(t);
			t=NULL;
		}
		Allocated=Length=0;
	}
	
	~AutoArray()
	{
		if (t)
			free(t);
	}
};
