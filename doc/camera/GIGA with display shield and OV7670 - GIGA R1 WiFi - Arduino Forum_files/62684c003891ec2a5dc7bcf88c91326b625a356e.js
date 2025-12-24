/* Translation data for theme 46 (en)*/
const data = {"en":{}};

for (let lang in data){
  let cursor = I18n.translations;
  for (let key of [lang, "js", "theme_translations"]){
    cursor = cursor[key] ??= {};
  }
  cursor[46] = data[lang];
}
