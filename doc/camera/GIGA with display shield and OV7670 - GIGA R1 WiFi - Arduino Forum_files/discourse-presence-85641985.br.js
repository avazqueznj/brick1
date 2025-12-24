define("discourse/plugins/discourse-presence/discourse/components/composer-presence-display",["exports","@glimmer/component","@glimmer/tracking","@ember/service","discourse/components/user-link","discourse/helpers/avatar","discourse/helpers/helper-fn","discourse/models/composer","discourse/truth-helpers","discourse-i18n","@ember/component","@ember/template-factory"],function(e,s,t,n,r,i,c,o,a,l,p,u){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
class d extends s.default{static{dt7948.g(this.prototype,"presence",[n.service])}#e=void dt7948.i(this,"presence")
static{dt7948.g(this.prototype,"composerPresenceManager",[n.service])}#s=void dt7948.i(this,"composerPresenceManager")
static{dt7948.g(this.prototype,"currentUser",[n.service])}#t=void dt7948.i(this,"currentUser")
static{dt7948.g(this.prototype,"siteSettings",[n.service])}#n=void dt7948.i(this,"siteSettings")
static{dt7948.g(this.prototype,"replyChannel",[t.tracked])}#r=void dt7948.i(this,"replyChannel")
static{dt7948.g(this.prototype,"whisperChannel",[t.tracked])}#i=void dt7948.i(this,"whisperChannel")
static{dt7948.g(this.prototype,"editChannel",[t.tracked])}#c=void dt7948.i(this,"editChannel")
static{dt7948.g(this.prototype,"translateChannel",[t.tracked])}#o=void dt7948.i(this,"translateChannel")
setupReplyChannel=(0,c.default)((e,s)=>{const{topic:t}=this.args.model
if(!t||!this.isReply)return
const n=`/discourse-presence/reply/${t.id}`,r=this.presence.getChannel(n)
this.replyChannel=r,r.subscribe(),s.cleanup(()=>r.unsubscribe())})
setupWhisperChannel=(0,c.default)((e,s)=>{const{topic:t}=this.args.model,{whisperer:n}=this.currentUser
if(!t||!this.isReply||!n)return
const r=`/discourse-presence/whisper/${t.id}`,i=this.presence.getChannel(r)
this.whisperChannel=i,i.subscribe(),s.cleanup(()=>i.unsubscribe())})
setupEditChannel=(0,c.default)((e,s)=>{const{post:t}=this.args.model
if(!t||!this.isEdit)return
const n=`/discourse-presence/edit/${t.id}`,r=this.presence.getChannel(n)
this.editChannel=r,r.subscribe(),s.cleanup(()=>r.unsubscribe())})
setupTranslateChannel=(0,c.default)((e,s)=>{const{post:t}=this.args.model
if(!t||!this.isTranslate)return
const n=`/discourse-presence/translate/${t.id}`,r=this.presence.getChannel(n)
this.translateChannel=r,r.subscribe(),s.cleanup(()=>r.unsubscribe())})
notifyState=(0,c.default)((e,s)=>{const{topic:t,post:n,replyDirty:r}=this.args.model,i=this.isEdit||this.isTranslate?n:t
if(i){const e=`/discourse-presence/${this.state}/${i.id}`
this.composerPresenceManager.notifyState(e,r)}s.cleanup(()=>this.composerPresenceManager.leave())})
get isReply(){return"reply"===this.state||"whisper"===this.state}get isEdit(){return"edit"===this.state}get isTranslate(){return"translate"===this.state}get state(){const{editingPost:e,whisper:s,replyingToTopic:t,action:n}=this.args.model
return n===o.default.ADD_TRANSLATION?"translate":e?"edit":s?"whisper":t?"reply":void 0}static{dt7948.n(this.prototype,"state",[t.cached])}get users(){let e
if(this.isEdit)e=this.editChannel?.users||[]
else if(this.isTranslate)e=this.translateChannel?.users||[]
else{e=[...this.replyChannel?.users||[],...this.whisperChannel?.users||[]]}return e.filter(e=>e.id!==this.currentUser.id).slice(0,this.siteSettings.presence_max_users_shown)}static{dt7948.n(this.prototype,"users",[t.cached])}static{(0,p.setComponentTemplate)((0,u.createTemplateFactory)({id:"BMhXA+PX",block:'[[[1,"\\n"],[41,[30,0,["currentUser"]],[[[1,"      "],[1,[30,0,["setupReplyChannel"]]],[1,"\\n      "],[1,[30,0,["setupWhisperChannel"]]],[1,"\\n      "],[1,[30,0,["setupEditChannel"]]],[1,"\\n      "],[1,[30,0,["setupTranslateChannel"]]],[1,"\\n      "],[1,[30,0,["notifyState"]]],[1,"\\n\\n"],[41,[28,[32,0],[[30,0,["users","length"]],0],null],[[[1,"        "],[10,0],[14,0,"presence-users"],[12],[1,"\\n          "],[10,0],[14,0,"presence-avatars"],[12],[1,"\\n"],[42,[28,[31,2],[[28,[31,2],[[30,0,["users"]]],null]],null],null,[[[1,"              "],[8,[32,1],null,[["@user"],[[30,1]]],[["default"],[[[[1,"\\n                "],[1,[28,[32,2],[[30,1]],[["imageSize"],["small"]]]],[1,"\\n              "]],[]]]]],[1,"\\n"]],[1]],null],[1,"          "],[13],[1,"\\n\\n          "],[10,1],[14,0,"presence-text"],[12],[1,"\\n            "],[10,1],[14,0,"description"],[12],[41,[30,0,["isReply"]],[[[1,[28,[32,3],["presence.replying"],[["count"],[[30,0,["users","length"]]]]]]],[]],[[[41,[30,0,["isTranslate"]],[[[1,[28,[32,3],["presence.translating"],[["count"],[[30,0,["users","length"]]]]]]],[]],[[[1,[28,[32,3],["presence.editing"],[["count"],[[30,0,["users","length"]]]]]]],[]]]],[]]],[13],[1,"\\n            "],[10,1],[14,0,"wave"],[12],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n            "],[13],[1,"\\n          "],[13],[1,"\\n        "],[13],[1,"\\n"]],[]],null]],[]],null],[1,"  "]],["user"],["if","each","-track-array"]]',moduleName:"/var/www/discourse/frontend/discourse/discourse/plugins/discourse-presence/discourse/components/composer-presence-display.js",scope:()=>[a.gt,r.default,i.default,l.i18n],isStrictMode:!0}),this)}}e.default=d}),define("discourse/plugins/discourse-presence/discourse/components/topic-presence-display",["exports","@glimmer/component","@glimmer/tracking","@ember/service","discourse/components/user-link","discourse/helpers/avatar","discourse/helpers/helper-fn","discourse/truth-helpers","discourse-i18n","@ember/component","@ember/template-factory"],function(e,s,t,n,r,i,c,o,a,l,p){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
class u extends s.default{static{dt7948.g(this.prototype,"presence",[n.service])}#e=void dt7948.i(this,"presence")
static{dt7948.g(this.prototype,"currentUser",[n.service])}#t=void dt7948.i(this,"currentUser")
static{dt7948.g(this.prototype,"replyChannel",[t.tracked])}#r=void dt7948.i(this,"replyChannel")
static{dt7948.g(this.prototype,"whisperChannel",[t.tracked])}#i=void dt7948.i(this,"whisperChannel")
setupReplyChannel=(0,c.default)((e,s)=>{const{topic:t}=this.args
if(!t)return
const n=`/discourse-presence/reply/${t.id}`,r=this.presence.getChannel(n)
this.replyChannel=r,r.subscribe(),s.cleanup(()=>r.unsubscribe())})
setupWhisperChannel=(0,c.default)((e,s)=>{const{topic:t}=this.args,{whisperer:n}=this.currentUser
if(!t||!n)return
const r=`/discourse-presence/whisper/${t.id}`,i=this.presence.getChannel(r)
this.whisperChannel=i,i.subscribe(),s.cleanup(()=>i.unsubscribe())})
get users(){return[...this.replyChannel?.users||[],...this.whisperChannel?.users||[]].filter(e=>e.id!==this.currentUser.id)}static{dt7948.n(this.prototype,"users",[t.cached])}static{(0,l.setComponentTemplate)((0,p.createTemplateFactory)({id:"gPqnSzAw",block:'[[[1,"\\n"],[41,[30,0,["currentUser"]],[[[1,"      "],[1,[30,0,["setupReplyChannel"]]],[1,"\\n      "],[1,[30,0,["setupWhisperChannel"]]],[1,"\\n\\n"],[41,[28,[32,0],[[30,0,["users","length"]],0],null],[[[1,"        "],[10,0],[14,0,"presence-users"],[12],[1,"\\n          "],[10,0],[14,0,"presence-avatars"],[12],[1,"\\n"],[42,[28,[31,2],[[28,[31,2],[[30,0,["users"]]],null]],null],null,[[[1,"              "],[8,[32,1],null,[["@user"],[[30,1]]],[["default"],[[[[1,"\\n                "],[1,[28,[32,2],[[30,1]],[["imageSize"],[[30,2]]]]],[1,"\\n              "]],[]]]]],[1,"\\n"]],[1]],null],[1,"          "],[13],[1,"\\n\\n          "],[10,1],[14,0,"presence-text"],[12],[1,"\\n            "],[10,1],[14,0,"description"],[12],[1,"\\n              "],[1,[28,[32,3],["presence.replying_to_topic"],[["count"],[[30,0,["users","length"]]]]]],[1,"\\n            "],[13],[1,"\\n            "],[10,1],[14,0,"wave"],[12],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n              "],[10,1],[14,0,"dot"],[12],[1,"."],[13],[1,"\\n            "],[13],[1,"\\n          "],[13],[1,"\\n        "],[13],[1,"\\n"]],[]],null]],[]],null],[1,"  "]],["user","@avatarSize"],["if","each","-track-array"]]',moduleName:"/var/www/discourse/frontend/discourse/discourse/plugins/discourse-presence/discourse/components/topic-presence-display.js",scope:()=>[o.gt,r.default,i.default,a.i18n],isStrictMode:!0}),this)}}e.default=u}),define("discourse/plugins/discourse-presence/discourse/connectors/before-composer-controls/presence",["exports","discourse/plugins/discourse-presence/discourse/components/composer-presence-display","@ember/component","@ember/template-factory","@ember/component/template-only"],function(e,s,t,n,r){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
const i=(0,t.setComponentTemplate)((0,n.createTemplateFactory)({id:"8ZBRgEHm",block:'[[[1,"\\n  "],[10,0],[14,0,"before-composer-controls-outlet presence"],[12],[1,"\\n    "],[8,[32,0],null,[["@model"],[[30,1,["model"]]]],null],[1,"\\n  "],[13],[1,"\\n"]],["@outletArgs"],[]]',moduleName:"/var/www/discourse/frontend/discourse/discourse/plugins/discourse-presence/discourse/connectors/before-composer-controls/presence.js",scope:()=>[s.default],isStrictMode:!0}),(0,r.default)(void 0,"presence:Presence"))
e.default=i}),define("discourse/plugins/discourse-presence/discourse/connectors/topic-above-footer-buttons/presence",["exports","@glimmer/component","@ember/helper","@ember/template","discourse/lib/avatar-utils","discourse/plugins/discourse-presence/discourse/components/topic-presence-display","@ember/component","@ember/template-factory"],function(e,s,t,n,r,i,c,o){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
const a="small"
class l extends s.default{get avatarDimensions(){return(0,r.translateSize)(a)}static{(0,c.setComponentTemplate)((0,o.createTemplateFactory)({id:"DsqpoTEd",block:'[[[1,"\\n    "],[10,0],[15,5,[28,[32,0],[[28,[32,1],["--avatar-min-height: ",[30,0,["avatarDimensions"]],"px"],null]],null]],[14,0,"topic-above-footer-buttons-outlet presence"],[12],[1,"\\n      "],[8,[32,2],null,[["@topic","@avatarSize"],[[30,1,["model"]],[32,3]]],null],[1,"\\n    "],[13],[1,"\\n  "]],["@outletArgs"],[]]',moduleName:"/var/www/discourse/frontend/discourse/discourse/plugins/discourse-presence/discourse/connectors/topic-above-footer-buttons/presence.js",scope:()=>[n.htmlSafe,t.concat,i.default,a],isStrictMode:!0}),this)}}e.default=l}),define("discourse/plugins/discourse-presence/discourse/services/composer-presence-manager",["exports","@ember/runloop","@ember/service","discourse/lib/environment"],function(e,s,t,n){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
class r extends t.default{static{dt7948.g(this.prototype,"currentUser",[t.service])}#t=void dt7948.i(this,"currentUser")
static{dt7948.g(this.prototype,"presence",[t.service])}#e=void dt7948.i(this,"presence")
notifyState(e,t=!0,r=1e4){t?this.currentUser.user_option.hide_presence||this._name!==e&&(this.leave(),this._name=e,this._channel=this.presence.getChannel(e),this._channel.enter(),(0,n.isTesting)()||(this._autoLeaveTimer=(0,s.debounce)(this,this.leave,r))):this.leave()}leave(){this._autoLeaveTimer&&((0,s.cancel)(this._autoLeaveTimer),this._autoLeaveTimer=null),this._channel?.leave(),this._channel=null,this._name=null}}e.default=r})

//# sourceMappingURL=discourse-presence-11fa6724.map