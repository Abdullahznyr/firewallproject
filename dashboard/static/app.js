async function jsonRequest(url, options = {}) {
  const response = await fetch(url, {
    headers: { "Content-Type": "application/json" },
    ...options,
  });
  const data = await response.json();
  if (!response.ok || data.ok === false) {
    throw new Error(data.error || "İşlem başarısız");
  }
  return data;
}

function showOutput(selector, value) {
  const el = document.querySelector(selector);
  if (el) el.textContent = typeof value === "string" ? value : JSON.stringify(value, null, 2);
}

const ruleForm = document.querySelector("#rule-form");
if (ruleForm) {
  ruleForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const form = new FormData(ruleForm);
    try {
      await jsonRequest("/api/rules", {
        method: "POST",
        body: JSON.stringify(Object.fromEntries(form.entries())),
      });
      location.reload();
    } catch (error) {
      document.querySelector("#rule-message").textContent = error.message;
    }
  });
}

document.querySelectorAll(".toggle-rule").forEach((button) => {
  button.addEventListener("click", async () => {
    await jsonRequest("/api/rules", {
      method: "PATCH",
      body: JSON.stringify({
        category: button.dataset.category,
        value: button.dataset.value,
        enabled: button.dataset.enabled === "true",
      }),
    });
    location.reload();
  });
});

document.querySelectorAll(".delete-rule").forEach((button) => {
  button.addEventListener("click", async () => {
    await jsonRequest("/api/rules", {
      method: "DELETE",
      body: JSON.stringify({
        category: button.dataset.category,
        value: button.dataset.value,
      }),
    });
    location.reload();
  });
});

const testForm = document.querySelector("#test-form");
if (testForm) {
  testForm.addEventListener("submit", async (event) => {
    event.preventDefault();
    const payload = Object.fromEntries(new FormData(testForm).entries());
    showOutput("#test-output", "Test çalışıyor...");
    try {
      const result = await jsonRequest("/api/test", {
        method: "POST",
        body: JSON.stringify(payload),
      });
      showOutput("#test-output", result);
    } catch (error) {
      showOutput("#test-output", error.message);
    }
  });
}

const refreshLogs = document.querySelector("#refresh-logs");
if (refreshLogs) {
  refreshLogs.addEventListener("click", async () => {
    const data = await jsonRequest("/api/logs");
    showOutput("#logs-output", data.logs.join("\n"));
  });
}

const applyRules = document.querySelector("#apply-rules");
if (applyRules) {
  applyRules.addEventListener("click", async () => {
    showOutput("#live-output", "Apply çalışıyor...");
    const result = await jsonRequest("/api/apply", { method: "POST", body: "{}" });
    showOutput("#live-output", result);
  });
}

const rollbackRules = document.querySelector("#rollback-rules");
if (rollbackRules) {
  rollbackRules.addEventListener("click", async () => {
    const result = await jsonRequest("/api/rollback", { method: "POST", body: "{}" });
    showOutput("#live-output", result);
  });
}

const emergencyReset = document.querySelector("#emergency-reset");
if (emergencyReset) {
  emergencyReset.addEventListener("click", async () => {
    const result = await jsonRequest("/api/emergency-reset", { method: "POST", body: "{}" });
    showOutput("#live-output", result);
  });
}

const simulationMode = document.querySelector("#simulation-mode");
if (simulationMode) {
  simulationMode.addEventListener("click", async () => {
    const result = await jsonRequest("/api/live-mode", {
      method: "POST",
      body: JSON.stringify({ enabled: false }),
    });
    showOutput("#live-output", result);
  });
}

const liveMode = document.querySelector("#live-mode");
if (liveMode) {
  liveMode.addEventListener("click", async () => {
    const result = await jsonRequest("/api/live-mode", {
      method: "POST",
      body: JSON.stringify({ enabled: true }),
    });
    showOutput("#live-output", result);
  });
}
