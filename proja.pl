#!/usr/bin/perl
$proj='def';
#$proj=$ARGV[0] if ($ARGV[0]);
foreach $opt (@ARGV)
{
#	push(@options,'-g') if ($opt eq '-g');
#	push(@options,'-pg') if ($opt eq '-p');
}
$options=join(' ',@options);
open(F,"$proj.prj") || die 'Fichier introuvable';
$commentaire=<F>;
$sortie=<F>;
chomp($sortie);
$defs=<F>;
chomp($defs);
$libs=<F>;
chomp($libs);
@fic=<F>;
chomp(@fic);
close(F);

foreach $fic (@fic)
{
	$fic=~s/(\.[^\/]*?$)//;
	$extension=$1;
	$extension='.cpp' unless $extension;
	
	$fico=$fic.'.o';
	$fico=~s/.*\///;
	push(@ficod,"Debug/$fico");
	push(@ficor,"Release/$fico");
	push(@ficc,$fic.$extension);
}

# mkdir('obj',0755);

open(G,">makefile.dbg");
open(H,">makefile.rls");
print G "exec = $sortie\n\n";
print G "parametres = $defs -D_DEBUG\n\n";
print G "biblio = $libs\n\n";
print G "objets = ",join(' ',@ficod),"\n\n";
print G "options = $options\n\n";
print G "\$(exec): \$(objets)\n\tg++ -g \$(options) -o \$\@ \$(objets) \$(biblio)\n\n";

print H "exec = $sortie\n\n";
print H "parametres = $defs\n\n";
print H "biblio = $libs\n\n";
print H "objets = ",join(' ',@ficor),"\n\n";
print H "options = $options\n\n";
print H "\$(exec): \$(objets)\n\tg++ -O \$(options) -o \$\@ \$(objets) \$(biblio)\n\n";

$i=0;
foreach $ficc (@ficc)
{
	print G 'Debug/';
	print G `g++ -MM $ficc`;
	print G "\tg++ -g \$(options) \$(parametres) -c -o $ficod[$i] $ficc\n\n";
	print H 'Release/';
	print H `g++ -MM $ficc`;
	print H "\tg++ -O \$(options) \$(parametres) -c -o $ficor[$i] $ficc\n\n";
	$i++;
}
close(G);
close(H);
open(G,">makefile");
print G <<"FIN";
default: debug
release:
	\${MAKE} -f makefile.rls
debug:
	\${MAKE} -f makefile.dbg
FIN
close(G);
print `make`;
