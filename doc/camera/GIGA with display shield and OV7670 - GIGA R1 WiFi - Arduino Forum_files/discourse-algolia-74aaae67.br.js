define("discourse/plugins/discourse-algolia/discourse/api-initializers/discourse-algolia",["exports","@ember/render-modifiers/modifiers/did-insert","rsvp","discourse/lib/api","discourse/lib/environment","discourse/lib/load-script","discourse/lib/url","discourse-i18n","@ember/component","@ember/template-factory","@ember/component/template-only"],function(e,a,s,t,i,l,o,r,n,c,u){"use strict"
Object.defineProperty(e,"__esModule",{value:!0}),e.default=void 0
e.default=(0,t.apiInitializer)(e=>{const t=e.container.lookup("service:site-settings"),d=e.getCurrentUser(),p=()=>t.algolia_enabled&&t.algolia_autocomplete_enabled&&(!t.login_required||d)
let g
function m(){g?.destroy(),s.Promise.all([(0,l.default)("/plugins/discourse-algolia/javascripts/autocomplete.js"),(0,l.default)("/plugins/discourse-algolia/javascripts/algoliasearch.js")]).then(()=>{document.body.classList.add("algolia-enabled"),g=function(e){const a=window.algoliasearch,{autocomplete:s,getAlgoliaResults:t}=window["@algolia/autocomplete-js"]
if(document.getElementsByClassName("algolia-search")[0].getElementsByClassName("aa-Autocomplete").length>0)return
const i=a(e.algoliaApplicationId,e.algoliaSearchApiKey),l=s({container:".algolia-search",panelContainer:".algolia-autocomplete",debug:e.debug,detachedMediaQuery:"none",placeholder:(0,r.i18n)("discourse_algolia.search_box_placeholder"),getSources:()=>[{sourceId:"posts",getItemInputValue:({item:e})=>e.query,getItems:({query:e})=>t({searchClient:i,queries:[{indexName:"discourse-posts",query:e,params:{hitsPerPage:4}}]}),templates:{item({item:e,components:a,html:s}){let t=[],i=e.topic.tags
return i&&i.forEach((i,r)=>{t.push(s`<a
                    class="hit-post-tag"
                    onClick="${e=>{o.default.routeTo(`/tags/${i}`),l.setIsOpen(!1),e.preventDefault(),e.stopPropagation()}}"
                  >
                    ${a.Highlight({hit:e,attribute:["topic","tags",r]})}
                  </a>`)}),s` <div class="hit-post">
                <div class="hit-post-title-holder">
                  <span class="hit-post-topic-title">
                    ${a.Highlight({hit:e,attribute:["topic","title"]})}
                  </span>
                  <span
                    class="hit-post-topic-views"
                    title="${(0,r.i18n)("discourse_algolia.topic_views")}"
                  >
                    ${e.topic.views}
                  </span>
                </div>
                <div class="hit-post-category-tags">
                  <span class="hit-post-category">
                    <span class="badge-wrapper bullet">
                      <span
                        class="badge-category-bg"
                        style="background-color: #${e.category?.color};"
                      />
                      <a
                        class="badge-category hit-post-category-name"
                        onClick="${a=>{o.default.routeTo(e.category.url),l.setIsOpen(!1),a.preventDefault(),a.stopPropagation()}}"
                        >${e.category?.name}</a
                      >
                    </span>
                  </span>
                  <span class="hit-post-tags">${t}</span>
                </div>
                <div class="hit-post-content-holder">
                  <a
                    class="hit-post-username"
                    onClick="${a=>{o.default.routeTo(e.user.url),l.setIsOpen(!1),a.preventDefault(),a.stopPropagation()}}"
                    >@${e.user.username}</a
                  >:
                  <span class="hit-post-content">
                    ${a.Snippet({hit:e,attribute:"content"})}
                  </span>
                </div>
              </div>`},noResults:({html:e})=>e`<div class="aa-empty">
                ${(0,r.i18n)("discourse_algolia.no_posts")}
              </div>`},onSelect({item:e}){o.default.routeTo(e.url)}},{sourceId:"users",getItemInputValue:({item:e})=>e.query,getItems:({query:e})=>t({searchClient:i,queries:[{indexName:"discourse-users",query:e,params:{hitsPerPage:4}}]}),templates:{item({item:a,components:s,html:t}){let i=""
a.likes_received>0&&(i=t`<span class="hit-user-like-heart">❤</span>
                  ${a.likes_received}`)
const l=s.Highlight({hit:a,attribute:a.name?"name":"username"})
return t`<div class="hit-user-left">
                  <img
                    class="hit-user-avatar"
                    src="${e.imageBaseURL}${a.avatar_template.replace("{size}",50)}"
                  />
                </div>
                <div class="hit-user-right">
                  <div class="hit-user-username-holder">
                    <span class="hit-user-username">
                      @${s.Highlight({hit:a,attribute:"username"})}
                    </span>
                    <span
                      class="hit-user-custom-ranking"
                      title="${(0,r.i18n)("discourse_algolia.user_likes")}"
                    >
                      ${i}
                    </span>
                  </div>
                  <div class="hit-user-name">${l}</div>
                </div>`}},onSelect({item:e}){o.default.routeTo(e.url)}},{sourceId:"tags",getItemInputValue:({item:e})=>e.query,getItems:({query:e})=>t({searchClient:i,queries:[{indexName:"discourse-tags",query:e,params:{hitsPerPage:4}}]}),templates:{item:({item:e,components:a,html:s})=>s`<div class="hit-tag">
                #<span class="hit-tag-name">
                  ${a.Highlight({hit:e,attribute:"name"})}</span
                >
                <span
                  class="hit-tag-topic_count"
                  title="${(0,r.i18n)("discourse_algolia.topic_tags")}"
                  >${e.topic_count}</span
                >
              </div> `},onSelect({item:e}){o.default.routeTo(e.url)}}],render({elements:e,render:a,html:s},t){const{posts:i,users:n,tags:c}=e
a(s`<div class="aa-dropdown-menu">
          <div class="left-container">
            <div class="aa-dataset-posts">${i}</div>
          </div>
          <div class="right-container">
            <span class="aa-dataset-users">${n}</span>
            <span class="aa-dataset-tags">${c}</span>
          </div>
          <div class="aa-footer">
            <div class="left-container">
              <a
                class="advanced-search"
                onClick="${e=>{o.default.routeTo("/search"),l.setIsOpen(!1),e.preventDefault(),e.stopPropagation()}}"
                >${(0,r.i18n)("discourse_algolia.advanced_search")}</a
              >
            </div>
            <div class="right-container">
              <a
                target="_blank"
                class="algolia-logo"
                href="https://algolia.com/"
                title="${(0,r.i18n)("discourse_algolia.powered_by")}"
              ></a>
            </div>
          </div>
        </div>`,t)}})
return l}({algoliaApplicationId:t.algolia_application_id,algoliaSearchApiKey:t.algolia_search_api_key,imageBaseURL:"",debug:(0,i.isDevelopment)()})})}e.headerIcons.add("algolia",(0,n.setComponentTemplate)((0,c.createTemplateFactory)({id:"TekbB3NW",block:'[[[1,"\\n"],[41,[28,[32,0],null,null],[[[1,"        "],[11,"li"],[24,0,"algolia-holder"],[4,[32,1],[[32,2]],null],[12],[1,"\\n          "],[10,0],[14,0,"algolia-search"],[12],[13],[1,"\\n          "],[10,0],[14,0,"algolia-autocomplete"],[12],[13],[1,"\\n        "],[13],[1,"\\n"]],[]],null],[1,"    "]],[],["if"]]',moduleName:"/var/www/discourse/frontend/discourse/discourse/plugins/discourse-algolia/discourse/api-initializers/discourse-algolia.js",scope:()=>[p,a.default,m],isStrictMode:!0}),(0,u.default)(void 0,void 0)))})})

//# sourceMappingURL=discourse-algolia-8ddbbb49.map