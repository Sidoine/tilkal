#ifndef TABLEAUX_INC
#define TABLEAUX_INC

template <class T>
class TTableau
{
public:
	T * t;
	size_t a;
	size_t l;
	
	void Detruire()
	{
		if (t)
			free(t);
		t=NULL;
		a=l=0;
	}
	
	TTableau()
	{
		l=a=0;
		t=NULL;
	}
	void Init()
	{
		l=a=0;
		t=NULL;
	}
	void Redim(size_t n)
	{
		if (n<a)
			return;
		a=n+128;
		t=(T*)realloc(t,sizeof(T)*a);
	}
	void Redim0(size_t n)
	{
		if (n<l)
			return;
		l=n;
		if (n<a)
			return;
		size_t z=a;
		a=n+128;
		t=(T*)realloc(t,sizeof(T)*a);
		memset(&t[z],0,sizeof(T)*(a-z));
	}
	void Redim02(size_t n)
	{
		if (n<a)
			return;
		size_t z=a;
		a=n+128;
		t=(T*)realloc(t,sizeof(T)*a);
		memset(&t[z],0,sizeof(T)*(a-z));
	}
	void Etendre()
	{
		if (l==a)
		{
			a+=128;
			t=(T*)realloc(t,sizeof(T)*a);
		}
		l++;
	}
	T & Dernier()
	{
		return t[l-1];
	}
	T & operator[](size_t i)
	{
		return t[i];
	}
	T & Premier()
	{
		return t[0];
	}
	T & Fin()
	{
		return t[l];
	}
};

//Tableau avec des trous
//T doit contenir un membre size_t "Vide"
template <class T>
class TTableauV
{
public:
	T * t;
	size_t a;
	size_t l;
	size_t Vide;
	
	TTableauV()
	{
		Vide=l=a=0;
		t=NULL;
	}
	void Init()
	{
		Vide=l=a=0;
		t=NULL;
	}

	size_t Nouveau()
	{
		size_t ret=Vide;
		if (Vide==l)
		{
			if (l==a)
			{
				a+=128;
				t=(T*)realloc(t,a*sizeof(T));
			}
			l++;
			Vide++;
		}
		else
			Vide=t[Vide].Vide;
		return ret;
	}

	void Supprimer(size_t i)
	{
		t[i].Vide=Vide;
		Vide=i;
	}

	T & operator[](size_t i)
	{
		return t[i];
	}
};

#endif
