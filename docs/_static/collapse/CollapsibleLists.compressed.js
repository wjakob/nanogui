/*

CollapsibleLists.js

An object allowing lists to dynamically expand and collapse

Created by Stephen Morley - http://code.stephenmorley.org/ - and released under
the terms of the CC0 1.0 Universal legal code:

http://creativecommons.org/publicdomain/zero/1.0/legalcode

*/

var CollapsibleLists=new function(){
this.apply=function(_1){
var _2=document.getElementsByTagName("ul");
for(var _3=0;_3<_2.length;_3++){
if(_2[_3].className.match(/(^| )collapsibleList( |$)/)){
this.applyTo(_2[_3],true);
if(!_1){
var _4=_2[_3].getElementsByTagName("ul");
for(var _5=0;_5<_4.length;_5++){
_4[_5].className+=" collapsibleList";
}
}
}
}
};
this.applyTo=function(_6,_7){
var _8=_6.getElementsByTagName("li");
for(var _9=0;_9<_8.length;_9++){
if(!_7||_6==_8[_9].parentNode){
if(_8[_9].addEventListener){
_8[_9].addEventListener("mousedown",function(e){
e.preventDefault();
},false);
}else{
_8[_9].attachEvent("onselectstart",function(){
event.returnValue=false;
});
}
if(_8[_9].addEventListener){
_8[_9].addEventListener("click",_a(_8[_9]),false);
}else{
_8[_9].attachEvent("onclick",_a(_8[_9]));
}
_b(_8[_9]);
}
}
};
function _a(_c){
return function(e){
if(!e){
e=window.event;
}
var _d=(e.target?e.target:e.srcElement);
while(_d.nodeName!="LI"){
_d=_d.parentNode;
}
if(_d==_c){
_b(_c);
}
};
};
function _b(_e){
var _f=_e.className.match(/(^| )collapsibleListClosed( |$)/);
var uls=_e.getElementsByTagName("ul");
for(var _10=0;_10<uls.length;_10++){
var li=uls[_10];
while(li.nodeName!="LI"){
li=li.parentNode;
}
if(li==_e){
uls[_10].style.display=(_f?"block":"none");
}
}
_e.className=_e.className.replace(/(^| )collapsibleList(Open|Closed)( |$)/,"");
if(uls.length>0){
_e.className+=" collapsibleList"+(_f?"Open":"Closed");
}
};
}();

