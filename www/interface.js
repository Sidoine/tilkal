var oRoom=document.getElementById("room");
var oObjects=document.getElementById("objects");
var oEvents=document.getElementById("events");
var oExits=document.getElementById("exits");
var oType=document.getElementById("type");
var oMenu=document.getElementById("menu");
var oInv=document.getElementById("inventaire");
var oGens=document.getElementById("gens");
var oBody=document.getElementById("body");
var oLastP;
var HistoCmd=new Array();
var PosDsHisto=0;
var myOId=0;

function YourId(id)
{
	myOId=id;
}

function Say(who, text)
{
	var p = document.createElement("P");
	p.className="dialogue";
	p.innerHTML="<strong>"+who+"</strong> "+text;
	oEvents.appendChild(p);
	ScrollP(p);
}

function YouSay(text)
{
	var p = document.createElement("P");
	p.className="dialogue";
	p.innerHTML="<strong>Vous</strong> "+text;
	oEvents.appendChild(p);
	ScrollP(p);
}

function Room(name, desc, image, id)
{
	oRoom.innerHTML=
		'<h1>'+name+'</h1><p>'+desc+'</p>';
	oRoom.myId=id;
	oObjects.innerHTML='';
	oGens.innerHTML='';
	oExits.innerHTML='';
}

function Exit(text, id)
{
	var p = document.createElement("SPAN");
	p.innerHTML='<a href="c,'+myId+'?t=go+'+id+'" target="z">'+text+'</a>&nbsp;';
	p.oncontextmenu=DisplayMenu;
	oMenu.innerHTML=
		'<a href="c,'+myId+'?t=go+'+id+'" target="z" class="godoor"></a>'
		+'<a href="c,'+myId+'?t=look+'+id+'" target="z" class="lookdoor"></a>';
	oMenu.style.width="64px";
	oExits.appendChild(p);
}

function DisplayMenu(e)
{
	if(document.all) e=window.event; // for IE
	/*if (e.button!=2)
		return;*/
	
	oMenu.style.display="block";
	oMenu.style.top=e.clientY+'px';
	oMenu.style.left=e.clientX+'px';
		
	if (e.preventBubble)
		e.preventBubble(); // for Mozilla
 	else
		e.cancelBubble = true;
	return false;
}

function HideMenu(e)
{
	if(document.all) e=window.event; // for IE
	oMenu.style.display="none";
}

/*function AddObj(text, howmany, image, id)
{
	var p = document.createElement("P");
	p.innerHTML=text+(howmany>1?'('+howmany+')':'');
	oObjects.appendChild(p);
}*/

function ChgQu(what, combien)
{
	var o=document.getElementById("qua"+what);
	if (o)
	{
		if (combien<=1)
			o.style.display='none';
		else
		{
			o.style.display='inline';
			var oTextNode = document.createTextNode('('+combien+')');
			o.firstChild.replaceNode(oTextNode);
		}
	}
}

function AddObj(what, whatname, whatimg, whattype, where, wherename,
	who, whoname, onwhat, howmany,total,held)
{
	var p = document.createElement("P");
	var chaine='<a href="c,'+myId+'?t=look+'+what+'" target="z">'+whatname+'</a>';
	chaine+=' <span id="qua'+what+'">';
	if (total>1)
		chaine+='('+total+')';
	chaine+='</span>';
	if (onwhat)
		chaine+=" ["+onwhat+"]";
	if (held)
		chaine+=' tenu';
	p.innerHTML=chaine;
	p.className=(whattype<2?"people":"object");
	p.myId=what;
	p.id="obj"+what;
	
	if (who!=-1)
	{
		var z = document.createElement("P");
		z.innerHTML=whoname+" a déplacé "+(howmany>1?howmany+" ":"")+
			whatname+" vers "+wherename;
		z.className='evbouge';
		oEvents.appendChild(z);
		ScrollP(z);
	}
	
	if (where==myOId)
	{
		oInv.appendChild(p);		
	}
	else if (where==oRoom.myId)
	{
		if (whattype<2)
			oGens.appendChild(p);
		else
			oObjects.appendChild(p);
	}
	else
	{
		var o=document.getElementById("obj"+where);
		if (o && o.parentNode!=oObjects && o.parentNode!=oGens)
		{
			o.appendChild(p);
		}
	}
}

function RemObj(id)
{
	var o=document.getElementById("obj"+id);
	if (o)
	{
		o.parentNode.removeChild(o);
	}
}

function Event(text)
{
	var p = document.createElement("P");
	p.innerHTML=text;
	oEvents.appendChild(p);
	ScrollP(p);
}

function Error(text)
{
	var p = document.createElement("P");
	p.className="error";
	p.innerHTML=text;
	oEvents.appendChild(p);
	ScrollP(p);
}


function Action()
{
	var cmd=oType.value;
	//On l'ajoute dans l'historique si elle n'y est pas déjà
	if (PosDsHisto<HistoCmd)
		HistoCmd.pop();
		
	if (HistoCmd.length==0 || HistoCmd[HistoCmd.length-1]!=cmd)
	{
		HistoCmd.push(cmd);
		if (HistoCmd.length>20)
			HistoCmd.shift();
	}
	PosDsHisto=HistoCmd.length;
	
	document.getElementById('t').value=cmd;
	oType.value='';
}

function ScrollP(p)
{
	oLastP=p;
	window.setTimeout("ScrollLastP()",0);
}

function ScrollLastP()
{
	oLastP.scrollIntoView();
}

function NavHisto(e)
{
	if(document.all) e=window.event; // for IE
	if (e.keyCode==38)
	{
		if (PosDsHisto>0)
		{
			if (PosDsHisto==HistoCmd.length)
				HistoCmd.push(oType.value);
			oType.value=HistoCmd[--PosDsHisto];
		}
	}
	else if (e.keyCode==40)
	{
		if (PosDsHisto<HistoCmd.length-1)
		{
			oType.value=HistoCmd[++PosDsHisto];
			if (PosDsHisto==HistoCmd.length-1)
				HistoCmd.pop();
		}
	}
}

function FermerMenu(e)
{
	if (document.all) e=window.event;
	if (oMenu.style.display=="block")
	{
		oMenu.style.display="none";
	}
}

function Rien(e)
{
	return false;
}

oType.onkeydown=NavHisto;
oBody.onclick=FermerMenu;
//oBody.oncontextmenu=Rien;

