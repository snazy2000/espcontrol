// Navigation folder: tap opens a nested grid screen with its own button layout
registerButtonType("subpage", {
  label: "Subpage",
  allowInSubpage: false,
  hideLabel: true,
  labelPlaceholder: "e.g. Lighting",
  onSelect: function (b) {
    b.entity = ""; b.sensor = ""; b.unit = ""; b.icon = "Auto"; b.icon_on = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    var mode = subpageStateDisplayMode(b);
    var showState = mode !== "off";
    var sensorEntity = b.sensor && b.sensor !== "indicator" ? b.sensor : "";
    var iconStateEntity = mode === "icon" ? (b.entity || "") : "";

    var labelSection = document.createElement("div");
    labelSection.className = "sp-field";
    labelSection.appendChild(helpers.fieldLabel("Label", helpers.idPrefix + "label"));
    var labelInp = helpers.textInput(helpers.idPrefix + "label", b.label, "e.g. Lighting");
    labelSection.appendChild(labelInp);
    panel.appendChild(labelSection);
    helpers.bindField(labelInp, "label", true);

    var iconSectionMain = helpers.makeIconPicker(
      helpers.idPrefix + "icon-picker", helpers.idPrefix + "icon",
      b.icon || "Auto", function (opt) {
        b.icon = opt;
        helpers.saveField("icon", opt);
      }, "Icon"
    );
    panel.appendChild(iconSectionMain);

    var showStateToggle = helpers.toggleRow("Show State", helpers.idPrefix + "state-toggle", showState);
    panel.appendChild(showStateToggle.row);

    var stateCond = condField();
    if (showState) stateCond.classList.add("sp-visible");

    var modeField = document.createElement("div");
    modeField.className = "sp-field";
    modeField.appendChild(helpers.fieldLabel("Display"));
    var modeSeg = document.createElement("div");
    modeSeg.className = "sp-segment";
    var iconBtn = document.createElement("button");
    iconBtn.type = "button";
    iconBtn.textContent = "Icon";
    var numericBtn = document.createElement("button");
    numericBtn.type = "button";
    numericBtn.textContent = "Numeric";
    var textBtn = document.createElement("button");
    textBtn.type = "button";
    textBtn.textContent = "Text";
    modeSeg.appendChild(iconBtn);
    modeSeg.appendChild(numericBtn);
    modeSeg.appendChild(textBtn);
    modeField.appendChild(modeSeg);
    stateCond.appendChild(modeField);

    var stateIconSection = condField();
    var iconEntityField = document.createElement("div");
    iconEntityField.className = "sp-field";
    iconEntityField.appendChild(helpers.fieldLabel("State Entity", helpers.idPrefix + "icon-state-entity"));
    var iconEntityInp = helpers.textInput(
      helpers.idPrefix + "icon-state-entity",
      iconStateEntity,
      "e.g. cover.office_blind"
    );
    iconEntityField.appendChild(iconEntityInp);
    stateIconSection.appendChild(iconEntityField);
    var onIconSection = helpers.makeIconPicker(
      helpers.idPrefix + "icon-on-picker", helpers.idPrefix + "icon-on",
      b.icon_on || "Auto", function (opt) {
        b.icon_on = opt;
        helpers.saveField("icon_on", opt);
      }, "On Icon"
    );
    stateIconSection.appendChild(onIconSection);
    stateCond.appendChild(stateIconSection);

    var sensorField = condField();
    var sf = document.createElement("div");
    sf.className = "sp-field";
    sf.appendChild(helpers.fieldLabel("Sensor Entity", helpers.idPrefix + "sensor"));
    var sensorInp = helpers.textInput(helpers.idPrefix + "sensor", sensorEntity, "e.g. sensor.open_windows");
    sf.appendChild(sensorInp);
    sensorField.appendChild(sf);
    helpers.requireField(sensorInp, "Add a sensor entity before saving.", function () {
      return showState && (mode === "numeric" || mode === "text");
    });

    function saveSensorEntity() {
      sensorEntity = sensorInp.value;
      if (showState && mode !== "icon") {
        b.sensor = sensorEntity;
        helpers.saveField("sensor", b.sensor);
      }
    }
    sensorInp.addEventListener("input", saveSensorEntity);
    sensorInp.addEventListener("change", saveSensorEntity);
    sensorInp.addEventListener("blur", saveSensorEntity);
    sensorInp.addEventListener("keydown", function (e) {
      if (e.key === "Enter") {
        saveSensorEntity();
        this.blur();
      }
    });

    function saveIconStateEntity() {
      iconStateEntity = iconEntityInp.value;
      if (showState && mode === "icon") {
        b.entity = iconStateEntity;
        helpers.saveField("entity", b.entity);
      }
    }
    iconEntityInp.addEventListener("input", saveIconStateEntity);
    iconEntityInp.addEventListener("change", saveIconStateEntity);
    iconEntityInp.addEventListener("blur", saveIconStateEntity);
    iconEntityInp.addEventListener("keydown", function (e) {
      if (e.key === "Enter") {
        saveIconStateEntity();
        this.blur();
      }
    });

    var numericSection = condField();

    var uf = document.createElement("div");
    uf.className = "sp-field";
    uf.appendChild(helpers.fieldLabel("Unit", helpers.idPrefix + "unit"));
    var unitInp = helpers.textInput(helpers.idPrefix + "unit", b.unit, "e.g. %");
    unitInp.className = "sp-input";
    uf.appendChild(unitInp);
    numericSection.appendChild(uf);
    helpers.bindField(unitInp, "unit", false);

    var pf = document.createElement("div");
    pf.className = "sp-field";
    pf.appendChild(helpers.fieldLabel("Unit Precision", helpers.idPrefix + "precision"));
    var precisionSelect = document.createElement("select");
    precisionSelect.className = "sp-select";
    precisionSelect.id = helpers.idPrefix + "precision";
    var precOpts = [["0", "10"], ["1", "10.2"], ["2", "10.21"]];
    for (var pi = 0; pi < precOpts.length; pi++) {
      var opt = document.createElement("option");
      opt.value = precOpts[pi][0];
      opt.textContent = precOpts[pi][1];
      precisionSelect.appendChild(opt);
    }
    precisionSelect.value = mode === "numeric" ? (b.precision || "0") : "0";
    precisionSelect.addEventListener("change", function () {
      if (mode !== "numeric") return;
      b.precision = this.value === "0" ? "" : this.value;
      helpers.saveField("precision", b.precision);
    });
    pf.appendChild(precisionSelect);
    numericSection.appendChild(pf);

    sensorField.appendChild(numericSection);
    stateCond.appendChild(sensorField);

    panel.appendChild(stateCond);

    function setMode(nextMode, persist) {
      mode = nextMode;
      showState = mode !== "off";
      showStateToggle.input.checked = showState;
      var iconLabel = iconSectionMain.querySelector(".sp-field-label");
      if (iconLabel) iconLabel.textContent = mode === "icon" ? "Off Icon" : "Icon";
      stateCond.classList.toggle("sp-visible", showState);
      iconBtn.classList.toggle("active", mode === "icon");
      numericBtn.classList.toggle("active", mode === "numeric");
      textBtn.classList.toggle("active", mode === "text");
      stateIconSection.classList.toggle("sp-visible", mode === "icon");
      sensorField.classList.toggle("sp-visible", mode === "numeric" || mode === "text");
      numericSection.classList.toggle("sp-visible", mode === "numeric");
      if (mode !== "numeric" && mode !== "text") helpers.clearFieldError(sensorInp);
      if (!persist) return;

      if (mode === "off") {
        b.sensor = "";
        b.entity = "";
        b.unit = "";
        b.precision = "";
        b.icon_on = "Auto";
        iconStateEntity = "";
        iconEntityInp.value = "";
        helpers.saveField("sensor", "");
        helpers.saveField("entity", "");
        helpers.saveField("unit", "");
        helpers.saveField("precision", "");
        helpers.saveField("icon_on", "Auto");
      } else if (mode === "icon") {
        b.sensor = "indicator";
        b.entity = iconStateEntity;
        b.unit = "";
        b.precision = "";
        helpers.saveField("sensor", "indicator");
        helpers.saveField("entity", b.entity);
        helpers.saveField("unit", "");
        helpers.saveField("precision", "");
      } else if (mode === "numeric") {
        b.sensor = sensorEntity;
        b.entity = "";
        b.unit = unitInp.value;
        b.precision = precisionSelect.value === "0" ? "" : precisionSelect.value;
        b.icon_on = "Auto";
        iconStateEntity = "";
        iconEntityInp.value = "";
        helpers.saveField("sensor", b.sensor);
        helpers.saveField("entity", "");
        helpers.saveField("unit", b.unit);
        helpers.saveField("precision", b.precision);
        helpers.saveField("icon_on", "Auto");
      } else if (mode === "text") {
        b.sensor = sensorEntity;
        b.entity = "";
        b.unit = "";
        b.precision = "text";
        b.icon_on = "Auto";
        iconStateEntity = "";
        iconEntityInp.value = "";
        helpers.saveField("sensor", b.sensor);
        helpers.saveField("entity", "");
        helpers.saveField("unit", "");
        helpers.saveField("precision", "text");
        helpers.saveField("icon_on", "Auto");
      }
    }

    iconBtn.addEventListener("click", function () { setMode("icon", true); });
    numericBtn.addEventListener("click", function () { setMode("numeric", true); });
    textBtn.addEventListener("click", function () { setMode("text", true); });
    showStateToggle.input.addEventListener("change", function () {
      setMode(this.checked ? (mode === "off" ? "icon" : mode) : "off", true);
    });
    setMode(mode, false);

    appendEditSubpageButton(panel, slot);
  },
  renderPreview: function (b, helpers) {
    var label = b.label || b.entity || "Configure";
    var mode = subpageStateDisplayMode(b);

    if (mode === "numeric") {
      var unit = b.unit ? helpers.escHtml(b.unit) : "";
      var prec = parseInt(b.precision || "0", 10) || 0;
      var sampleVal = (0).toFixed(prec);
      return {
        iconHtml:
          '<span class="sp-sensor-preview">' +
            '<span class="sp-sensor-value">' + sampleVal + '</span>' +
            '<span class="sp-sensor-unit">' + unit + '</span>' +
          '</span>',
        labelHtml:
          '<span class="sp-btn-label-row"><span class="sp-btn-label">' +
            helpers.escHtml(b.label || b.sensor || "Subpage") +
          '</span><span class="sp-subpage-badge mdi mdi-chevron-right"></span></span>',
      };
    }

    if (mode === "text") {
      var iconName = b.icon && b.icon !== "Auto" ? iconSlug(b.icon) : "cog";
      return {
        iconHtml: '<span class="sp-btn-icon mdi mdi-' + iconName + '"></span>',
        labelHtml:
          '<span class="sp-btn-label-row"><span class="sp-btn-label">State</span>' +
          '<span class="sp-subpage-badge mdi mdi-chevron-right"></span></span>',
      };
    }

    return {
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">' + helpers.escHtml(label) + '</span>' +
        '<span class="sp-subpage-badge mdi mdi-chevron-right"></span></span>',
    };
  },
  contextMenuItems: function (slot, b, helpers) {
    helpers.addCtxItem("cog", "Edit Subpage", function () { enterSubpage(slot); });
  },
});

function appendEditSubpageButton(panel, slot) {
  var configBtn = document.createElement("button");
  configBtn.className = "sp-action-btn sp-edit-subpage-btn";
  configBtn.textContent = "Edit Subpage";
  configBtn.addEventListener("click", function () { closeSettings(); enterSubpage(slot); });
  panel.appendChild(configBtn);
}
