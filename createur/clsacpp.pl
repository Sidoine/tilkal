#!/usr/bin/perl

open(F,"createur.cls");
undef $/;
$source=<F>;
close(F);

$/="\n";

$source=~ s/\/\/.*$//gm;
$source=~ s/\/\*.*?\*\///gs;

open(F,">createur_xml.cpp");
open(H,">createur_xml.h");

print F <<"FIN";
#include "../xml.h"
#include "createur_xml.h"

FIN

$source=~ s/enum\s+(\w+)\s*\{(.*?)\}/&Enum($1,$2);/ges;
$source=~ s/class\s+(\w+)\s*\{(.*?)\}/&Classe($1,$2);/ges;

close(F);
close(H);

sub Enum
{
	my ($nom,$texte)=@_;
	print H "enum $nom\n{\n$texte\n};\n";
	$enum{$nom}=[split(/\s*,\s*/,$texte)];
}

sub Classe
{
	my ($nom,$texte)=@_;
	my @var=split(';',$texte);
	my %defaut;
	my %type;
	push(@classe,$nom);
	
	foreach $v (@var)
	{
		if ($v=~/^\s*(\w+)\s*(\w+)(?:\s*=\s*".*?")\s*$/)
		{
			$type{$2}=$1;
			$type{$2}=$3;
		}
		else
		{
			die "Erreur de syntaxe : $v dans la classe $nom\n";
		}
	
	}
	
	print H <<"DEBUT";
class XML$nom:public CObjet
{
public:
	void Ajouter(char * _Clef, int lClef, char * _Valeur, int lValeur);
	XML$nom(CObjet * _Pere, CXML * _XML):CObjet(_Pere,_XML)
	{
DEBUT
	foreach $v (keys %type)
	{
		if ($type eq 'string')
		{
			if ($defaut{$v})
			{
				print H "\t\t$v=\"$defaut{$v}\";\n";
			}
			else
			{
				print H "\t\t$v=NULL;\n";
			}
		}
		elsif ($type =~ /^id<(.*)>$/)
		{
			print F "\t\t${v}=NULL;\n";
		}
		elsif ($type eq 'int')
		{
			if ($defaut{$v})
			{
				print H "\t\t$v=$defaut{$v};\n";
			}
			else
			{
				print H "\t\t$v=0;\n";
			}
		}
		elsif ($type eq 'Interv<int>')
		{
			if ($defaut{$v}=~/(\d*)-(\d*)/)
			{
				if (defined $1)
				{
					if (defined $2)
					{
						print H "\t\t$v.MinMax($1,$2);\n";
					}
					else
					{
						print H "\t\t$v.Min($1);\n";
					}
				}
				else
				{
					print H "\t\t$v.Max($2);\n";
				}
			}
			else
			{
				print H "\t\t$v.NoLimit();\n";
			}
		}
	}
	print H <<"DEBUT";
	}
	
	~XML$nom()
	{
DEBUT
	foreach $v (keys %type)
	{
		if ($type eq 'string')
		{
			print H "\t\tif ($v)\n\t\t\tdelete $v;\n";
		}
	}
	
	print H "\t}\n\n";
	
	foreach $v (keys %type)
	{
		if ($type eq 'string')
		{
			print H "\tchar * $v;\n";
		}
		elsif ($type =~ /^id<(.*)>$/)
		{
			print H "\tXML$1 $v\n";
		}
		elsif ($type eq 'int')
		{
			print H "\tint $v;\n";
		}
		elsif ($type eq 'Interv<int>')
		{
			print H "\tInterv<int> $v;\n";
		}
	}
	print H "};\n\n;";
	
	print F "void XML$v::Ajouter(char *_Clef, int lClef, char *_Valeur, int lValeur)\n{\n";
	foreach $v (keys %type)
	{
		print H "\tif (EGALES2(_Clef,lClef,\"$v\"))\n";
		if ($type eq 'string')
		{
			print H <<"FIN";
	{
		$v=new char[lValeur];
		memcpy($v,_Valeur,lValeur);
		$v[lValeur]=0;
	}
FIN
		}
		elsif ($type eq 'int')
		{
			print H "\t\t$v=atoi(_Valeur);\n";
		}
		elsif ($type=~ /^id<(.*)>$/)
		{
			print H <<"FIN";
	{
		if (lValeur>TMAXCLEFHACHAGE)
			lValeur=TMAXCLEFHACHAGE;
		$v=Index.
	}
FIN
		}
		elsif ($type eq 'Interv<int>')
		{
			print H <<"FIN";
	{
		int lmin=0;
		int min=0;
		while (lmin<lValeur && _Valeur[lmin]!='-')
		{
			min=min*10+_Valeur[l]-'0';
			lmin++;
		}
		int lmax=0;
		int max=0;
		while (lmin+1+lmax<lValeur)
		{
			min=min*10+_Valeur[lmin+lmax+1]-'0';
			lmax++;
		}
		if (lmin)
		{
			if (lmax)
				$v.MinMax(min,max);
			else
				$v.Min(min);
		}
		else
			$v.Max(max);
	}
FIN
		}
		print H "\telse\n";
	}
	print H "\t\tCObjet::Ajouter(_Clef,lClef,_Valeur,lValeur);\n}\n";
}
