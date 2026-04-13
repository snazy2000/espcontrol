registerButtonType("subpage", {
  label: "Subpage",
  allowInSubpage: false,
  labelPlaceholder: "e.g. Lighting",
  onSelect: function (b) {
    b.entity = ""; b.sensor = ""; b.unit = ""; b.icon_on = "Auto";
  },
  renderSettings: function (panel, b, slot, helpers) {
    panel.appendChild(helpers.makeIconPicker(
      helpers.idPrefix + "icon-picker", helpers.idPrefix + "icon",
      b.icon || "Auto", function (opt) {
        b.icon = opt;
        helpers.saveField("icon", opt);
        renderPreview();
      }
    ));
    var hasIconOn = b.icon_on && b.icon_on !== "Auto";
    var whenOnEnabled = b.sensor === "indicator" || hasIconOn;

    var whenOnToggle = helpers.toggleRow("When Entity On", helpers.idPrefix + "whenon-toggle", whenOnEnabled);
    panel.appendChild(whenOnToggle.row);

    var whenOnCond = condField();
    if (whenOnEnabled) whenOnCond.classList.add("sp-visible");

    var iconOnSection = document.createElement("div");
    iconOnSection.className = "sp-field";
    iconOnSection.appendChild(helpers.fieldLabel("Icon When On", helpers.idPrefix + "icon-on"));
    var iconOnVal = hasIconOn ? b.icon_on : "Auto";
    var iconOnPicker = document.createElement("div");
    iconOnPicker.className = "sp-icon-picker";
    iconOnPicker.id = helpers.idPrefix + "icon-on-picker";
    iconOnPicker.innerHTML =
      '<span class="sp-icon-picker-preview mdi mdi-' + iconSlug(iconOnVal) + '"></span>' +
      '<input class="sp-icon-picker-input" id="' + helpers.idPrefix + 'icon-on" type="text" ' +
      'placeholder="Search icons\u2026" value="' + escAttr(iconOnVal) + '" autocomplete="off">' +
      '<div class="sp-icon-dropdown"></div>';
    iconOnSection.appendChild(iconOnPicker);
    whenOnCond.appendChild(iconOnSection);

    initIconPicker(iconOnPicker, iconOnVal, function (opt) {
      b.icon_on = opt;
      helpers.saveField("icon_on", opt);
      renderPreview();
    });

    panel.appendChild(whenOnCond);

    whenOnToggle.input.addEventListener("change", function () {
      if (this.checked) {
        b.sensor = "indicator";
        helpers.saveField("sensor", "indicator");
        whenOnCond.classList.add("sp-visible");
      } else {
        b.sensor = "";
        b.icon_on = "Auto";
        helpers.saveField("sensor", "");
        helpers.saveField("icon_on", "Auto");
        whenOnCond.classList.remove("sp-visible");
        var ionPreview = iconOnPicker.querySelector(".sp-icon-picker-preview");
        if (ionPreview) ionPreview.className = "sp-icon-picker-preview mdi mdi-cog";
        var ionInput = iconOnPicker.querySelector(".sp-icon-picker-input");
        if (ionInput) ionInput.value = "Auto";
        renderPreview();
      }
    });

    var configBtn = document.createElement("button");
    configBtn.className = "sp-action-btn";
    configBtn.style.background = "var(--accent)";
    configBtn.style.color = "#fff";
    configBtn.style.width = "100%";
    configBtn.textContent = "Configure Subpage";
    configBtn.addEventListener("click", function () { enterSubpage(slot); });
    panel.appendChild(configBtn);
  },
  renderPreview: function (b, helpers) {
    var label = b.label || b.entity || "Configure";
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
