define("discourse/plugins/discourse-bbcode-color/lib/discourse-markdown/bbcode-color",["exports"],function(o){"use strict"
Object.defineProperty(o,"__esModule",{value:!0}),o.setup=function(o){o.allowList({custom(o,e,t){if("span"===o&&"style"===e)return/^(background-)?color:#?[a-zA-Z0-9]+$/.exec(t)}}),o.registerOptions(o=>{o.features["bbcode-color"]=!0}),o.markdownIt?o.registerPlugin(o=>{const e=o.inline.bbcode.ruler
e.push("bgcolor",{tag:"bgcolor",wrap:function(o,e,t){o.type="span_open",o.tag="span",o.attrs=[["style","background-color:"+t.attrs._default.trim()]],o.content="",o.nesting=1,e.type="span_close",e.tag="span",e.nesting=-1,e.content=""}}),e.push("color",{tag:"color",wrap:function(o,e,t){o.type="span_open",o.tag="span",o.attrs=[["style","color:"+t.attrs._default.trim()]],o.content="",o.nesting=1,e.type="span_close",e.tag="span",e.nesting=-1,e.content=""}})}):(o.addPreProcessor(o=>function(o){let e
o||=""
do{e=o,o=o.replace(/\[color=([^\]]+)\]((?:(?!\[color=[^\]]+\]|\[\/color\])[\S\s])*)\[\/color\]/gi,(o,e,t)=>`<span style='color:${e}'>${t}</span>`)}while(o!==e)
return o}(o)),o.addPreProcessor(o=>function(o){let e
o||=""
do{e=o,o=o.replace(/\[bgcolor=([^\]]+)\]((?:(?!\[bgcolor=[^\]]+\]|\[\/bgcolor\])[\S\s])*)\[\/bgcolor\]/gi,(o,e,t)=>`<span style='background-color:${e}'>${t}</span>`)}while(o!==e)
return o}(o)))}})

//# sourceMappingURL=discourse-bbcode-color-2a760d31.map