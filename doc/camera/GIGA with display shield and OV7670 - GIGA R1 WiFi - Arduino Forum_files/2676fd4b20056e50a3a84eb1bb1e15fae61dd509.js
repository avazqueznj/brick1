!function () {
  var analytics = window.analytics = window.analytics || [];
  if (!analytics.initialize) if (analytics.invoked) window.console && console.error && console.error("Segment snippet included twice.");else {
    analytics.invoked = !0;
    analytics.methods = ["trackSubmit", "trackClick", "trackLink", "trackForm", "pageview", "identify", "reset", "group", "track", "ready", "alias", "debug", "page", "once", "off", "on", "addSourceMiddleware", "addIntegrationMiddleware", "setAnonymousId", "addDestinationMiddleware"];
    analytics.factory = function (e) {
      return function () {
        var t = Array.prototype.slice.call(arguments);
        t.unshift(e);
        analytics.push(t);
        return analytics;
      };
    };
    for (var e = 0; e < analytics.methods.length; e++) {
      var key = analytics.methods[e];
      analytics[key] = analytics.factory(key);
    }
    analytics.load = function (key, e) {
      var t = document.createElement("script");
      t.type = "text/javascript";
      t.async = !0;
      t.src = "https://evs.aayinltcs.arduino.cc/5yy37B6M2UBwaTzKNi37Ls/qUh2Pce5EsyeGTFGda1mDz.min.js";
      var n = document.getElementsByTagName("script")[0];
      n.parentNode.insertBefore(t, n);
      analytics._loadOptions = e;
    };
    analytics._writeKey = "Mjvf0GrvmQYNDH0GfIhND5e8aGFynScR";
    analytics._cdn = "https://evs.aayinltcs.arduino.cc";
    analytics.SNIPPET_VERSION = "4.15.3";
    analytics.load("Mjvf0GrvmQYNDH0GfIhND5e8aGFynScR");
  }
}();
//# sourceMappingURL=2676fd4b20056e50a3a84eb1bb1e15fae61dd509.map?__ws=forum.arduino.cc
