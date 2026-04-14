registerButtonType("", {
  label: "Switch",
  allowInSubpage: true,
  renderPreview: function (b, helpers) {
    var label = b.label || b.entity || "Configure";
    return {
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">' + helpers.escHtml(label) + '</span>' +
        '<span class="sp-subpage-badge mdi mdi-toggle-switch-variant-off"></span></span>',
    };
  },
});
