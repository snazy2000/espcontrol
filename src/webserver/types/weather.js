// Read-only weather card: displays a HA weather condition as icon + label.
registerButtonType("weather", {
  label: "Weather",
  allowInSubpage: true,
  hideLabel: true,
  onSelect: function (b) {
    b.label = "";
    b.icon = "Auto";
    b.icon_on = "Auto";
    b.sensor = "";
    b.unit = "";
    b.precision = "";
  },
  renderSettings: function (panel, b, slot, helpers) {
    var ef = document.createElement("div");
    ef.className = "sp-field";
    ef.appendChild(helpers.fieldLabel("Weather Entity", helpers.idPrefix + "entity"));
    var entityInp = helpers.textInput(helpers.idPrefix + "entity", b.entity, "e.g. weather.forecast_home");
    ef.appendChild(entityInp);
    panel.appendChild(ef);
    helpers.bindField(entityInp, "entity", true);
  },
  renderPreview: function (b, helpers) {
    return {
      iconHtml: '<span class="sp-btn-icon mdi mdi-weather-cloudy"></span>',
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">Cloudy</span>' +
        '<span class="sp-type-badge mdi mdi-weather-cloudy"></span></span>',
    };
  },
});
