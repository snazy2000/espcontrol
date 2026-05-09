// Camera card: displays live snapshots from Home Assistant camera entities
// Fetches JPEG snapshots from /api/camera_proxy/{entity_id} and refreshes on interval
registerButtonType("camera", {
  label: "Camera",
  allowInSubpage: true,
  hideLabel: false,
  labelPlaceholder: "e.g. Front Door",
  onSelect: function (b) {
    b.entity = "";
    b.sensor = ""; // refresh interval in seconds, empty = on-demand
    b.unit = ""; // aspect ratio hint, e.g. "16:9"
    b.icon = "Auto";
    if (!b.precision) b.precision = ""; // "fit" or "fill"
  },
  renderSettings: function (panel, b, slot, helpers) {
    // Camera entity selection
    var ef = document.createElement("div");
    ef.className = "sp-field";
    ef.appendChild(helpers.fieldLabel("Camera Entity", helpers.idPrefix + "camera"));
    var entityInp = helpers.textInput(helpers.idPrefix + "camera", b.entity, "e.g. camera.front_door");
    ef.appendChild(entityInp);
    panel.appendChild(ef);
    helpers.bindField(entityInp, "entity", true);
    helpers.requireField(entityInp, "Add a camera entity before saving.");

    // Label
    var lf = document.createElement("div");
    lf.className = "sp-field";
    lf.appendChild(helpers.fieldLabel("Label", helpers.idPrefix + "label"));
    var labelInp = helpers.textInput(helpers.idPrefix + "label", b.label, "e.g. Front Door Camera");
    lf.appendChild(labelInp);
    panel.appendChild(lf);
    helpers.bindField(labelInp, "label", true);

    // Icon
    var iconPicker = helpers.makeIconPicker(
      helpers.idPrefix + "icon-picker", helpers.idPrefix + "icon",
      b.icon || "Auto", function (opt) {
        b.icon = opt;
        helpers.saveField("icon", opt);
      }
    );
    panel.appendChild(iconPicker);

    // Refresh interval
    var rf = document.createElement("div");
    rf.className = "sp-field";
    rf.appendChild(helpers.fieldLabel("Refresh Interval (seconds)", helpers.idPrefix + "refresh"));
    var refreshInp = helpers.textInput(helpers.idPrefix + "refresh", b.sensor || "", "e.g. 5 (empty = manual)");
    refreshInp.type = "number";
    refreshInp.min = "1";
    refreshInp.max = "300";
    rf.appendChild(refreshInp);
    panel.appendChild(rf);
    helpers.bindField(refreshInp, "sensor", true);

    // Aspect ratio
    var af = document.createElement("div");
    af.className = "sp-field";
    af.appendChild(helpers.fieldLabel("Aspect Ratio", helpers.idPrefix + "aspect"));
    var aspectSelect = document.createElement("select");
    aspectSelect.className = "sp-select";
    aspectSelect.id = helpers.idPrefix + "aspect";
    var aspectOpts = [
      ["", "Auto"],
      ["16:9", "16:9 (Widescreen)"],
      ["4:3", "4:3 (Standard)"],
      ["1:1", "1:1 (Square)"],
      ["21:9", "21:9 (Ultrawide)"],
    ];
    aspectOpts.forEach(function (entry) {
      var opt = document.createElement("option");
      opt.value = entry[0];
      opt.textContent = entry[1];
      aspectSelect.appendChild(opt);
    });
    aspectSelect.value = b.unit || "";
    aspectSelect.addEventListener("change", function () {
      b.unit = this.value;
      helpers.saveField("unit", this.value);
    });
    af.appendChild(aspectSelect);
    panel.appendChild(af);

    // Scaling mode
    var sf = document.createElement("div");
    sf.className = "sp-field";
    sf.appendChild(helpers.fieldLabel("Scaling", helpers.idPrefix + "scaling"));
    var scalingSelect = document.createElement("select");
    scalingSelect.className = "sp-select";
    scalingSelect.id = helpers.idPrefix + "scaling";
    var scalingOpts = [
      ["fit", "Fit (preserve aspect)"],
      ["fill", "Fill (crop to fit)"],
    ];
    scalingOpts.forEach(function (entry) {
      var opt = document.createElement("option");
      opt.value = entry[0];
      opt.textContent = entry[1];
      scalingSelect.appendChild(opt);
    });
    scalingSelect.value = b.precision || "fit";
    scalingSelect.addEventListener("change", function () {
      b.precision = this.value;
      helpers.saveField("precision", this.value);
    });
    sf.appendChild(scalingSelect);
    panel.appendChild(sf);
  },
  renderPreview: function (b, helpers) {
    var label = b.label || "Camera";
    var iconName = b.icon && b.icon !== "Auto" ? iconSlug(b.icon) : "camera";
    return {
      iconHtml:
        '<span class="sp-camera-preview">' +
          '<span class="mdi mdi-' + iconName + '" style="font-size: 3em; opacity: 0.5;"></span>' +
          '<span style="font-size: 0.8em; opacity: 0.7; margin-top: 0.5em;">Live Camera</span>' +
        '</span>',
      labelHtml:
        '<span class="sp-btn-label-row"><span class="sp-btn-label">' + helpers.escHtml(label) + '</span>' +
        '<span class="sp-type-badge mdi mdi-camera"></span></span>',
    };
  },
});
