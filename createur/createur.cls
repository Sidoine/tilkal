class Salle
{
	string titre;
};

enum PositionSalle
{
	entree
};

class AvecSalle
{
	id<Salle> quoi;
	PositionSalle position;
};

class AvecPorte
{
	id<Porte> quoi;
};

class Borde
{
	id<Salle> par;
};

enum Genre
{
	f,m,fp,mp
};

class Porte
{
	string titre;
	Genre genre="m";
	Interv<int> pente="-";
};
