const MIN_SPEED = 1.5;
const MAX_SPEED = 2.5;

if (typeof CryptoJS === 'undefined') {
  var CryptoJS = require('crypto-js');
}

function randomNumber(min, max) {
  return Math.random() * (max - min) + min;
}

class Blob {
  constructor(el) {
    this.el = el;
    const boundingRect = this.el.getBoundingClientRect();
    this.size = boundingRect.width;
    this.initialX = randomNumber(0, window.innerWidth - this.size);
    this.initialY = randomNumber(0, window.innerHeight - this.size);
    this.el.style.top = `${this.initialY}px`;
    this.el.style.left = `${this.initialX}px`;
    this.vx = randomNumber(MIN_SPEED, MAX_SPEED) * (Math.random() > 0.5 ? 1 : -1);
    this.vy = randomNumber(MIN_SPEED, MAX_SPEED) * (Math.random() > 0.5 ? 1 : -1);
    this.x = this.initialX;
    this.y = this.initialY;
  }

  update() {
    this.x += this.vx;
    this.y += this.vy;
    if (this.x >= window.innerWidth - this.size) {
      this.x = window.innerWidth - this.size;
      this.vx *= -1;
    }
    if (this.y >= window.innerHeight - this.size) {
      this.y = window.innerHeight - this.size;
      this.vy *= -1;
    }
    if (this.x <= 0) {
      this.x = 0;
      this.vx *= -1;
    }
    if (this.y <= 0) {
      this.y = 0;
      this.vy *= -1;
    }
  }

  move() {
    this.el.style.transform = `translate(${this.x - this.initialX}px, ${this.y - this.initialY}px)`;
  }
}

function initBlobs() {
  const blobEls = document.querySelectorAll(".bouncing-blob");
  const blobs = Array.from(blobEls).map((blobEl) => new Blob(blobEl));

  function update() {
    requestAnimationFrame(update);
    blobs.forEach((blob) => {
      blob.update();
      blob.move();
    });
  }

  requestAnimationFrame(update);
}

function setupRegisterForm() {
  document.getElementById("create-account-link").addEventListener("click", toggleForm);
  document.getElementById("login-link").addEventListener("click", toggleForm);

  document.getElementById("submit-button").addEventListener("click", function (event) {
    const username = document.getElementById("username").value;
    const password = document.getElementById("password").value;
    const repeatPassword = document.getElementById("repeat-password").value;

    if (this.textContent === "Registrarse") {
      if (password !== repeatPassword) {
        document.getElementById("password-error").style.display = "block";
        event.preventDefault();
      } else {
        document.getElementById("password-error").style.display = "none";
        registerUser(username, password);
        event.preventDefault();
      }
    } else {
      loginUser(username, password);
      event.preventDefault();
    }
  });
}

function toggleForm() {
  const isLogin = document.getElementById("submit-button").textContent.includes("Log In");

  if (isLogin) {
    document.getElementById("repeat-password").style.display = "block";
    document.getElementById("submit-button").textContent = "Registrarse";
    document.getElementById("no-account-text").style.display = "none";
    document.getElementById("create-account-link").style.display = "none";
    document.getElementById("has-account-text").style.display = "inline";
    document.getElementById("login-link").style.display = "inline";
  } else {
    document.getElementById("repeat-password").style.display = "none";
    document.getElementById("submit-button").textContent = "Log In";
    document.getElementById("no-account-text").style.display = "inline";
    document.getElementById("create-account-link").style.display = "inline";
    document.getElementById("has-account-text").style.display = "none";
    document.getElementById("login-link").style.display = "none";
    document.getElementById("password-error").style.display = "none";
    document.getElementById("login-error").style.display = "none";
    document.getElementById("register-error").style.display = "none";
  }
}

async function registerUser(username, password) {
  try {
    const response = await axios.post('https://europe-southwest1-tfg-borjadominguez.cloudfunctions.net/function-login-register', {
      action: 'register',
      username,
      password
    });
    alert(response.data.message);
  } catch (error) {
    console.error('Error al registrar usuario:', error);
    showError('register-error');
  }
}

async function loginUser(username, password) {
  try {
    const response = await axios.post('https://europe-southwest1-tfg-borjadominguez.cloudfunctions.net/function-login-register', {
      action: 'login',
      username,
      password
    });
    if (response.data.message === 'Inicio de sesión exitoso.') {
      showQRCodeScreen(username);
    } else {
      showError('login-error');
    }
  } catch (error) {
    console.error('Error al iniciar sesión:', error);
    showError('login-error');
  }
}

function showError(errorId) {
  const errorElement = document.getElementById(errorId);
  if (errorElement) {
    errorElement.style.display = 'block';
  }
}

async function triggerQRExpireFunction(payload) {
  try {
    const response = await axios.post('https://us-central1-tfg-borjadominguez.cloudfunctions.net/qr_expire', { payload }, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('QR expire function triggered:', response.data);
  } catch (error) {
    console.error('Error triggering qr_expire function:', error.response ? error.response.data : error.message);
  }
}

// Generar y mostrar el código QR con el payload del usuario.
function showQRCodeScreen(username) {
  const loginContainer = document.querySelector('.login-container');
  let payload = generatePayload(username);
  let hashedPayload = CryptoJS.SHA256(payload).toString(); // Hashear el payload
  savePayloadToFirestore(username, hashedPayload);
  triggerQRExpireFunction(hashedPayload); // Llamar a la función qr_expire

  loginContainer.innerHTML = `
      <h2>Tu código QR</h2>
      <img id="qr-code" src="https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${hashedPayload}" alt="Código QR">
      <button id="new-qr-button">Nuevo Código QR</button>
      <button id="logout-button">Log Out</button>
      <a href="#" class="UAH-firma">Grado en Ingeniería Telemática <br>Universidad de Alcalá <br>2017 - 2024</a>
  `;

  document.getElementById("logout-button").addEventListener("click", () => {
    location.reload();
  });

  document.getElementById("new-qr-button").addEventListener("click", () => {
    payload = generatePayload(username);
    hashedPayload = CryptoJS.SHA256(payload).toString(); // Hashear el nuevo payload
    savePayloadToFirestore(username, hashedPayload);
    triggerQRExpireFunction(hashedPayload); // Llamar a la función qr_expire para el nuevo payload
    document.getElementById("qr-code").src = `https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${hashedPayload}`;
  });

  // Refrescar el QR cada 10 segundos
  setInterval(() => {
    payload = generatePayload(username);
    hashedPayload = CryptoJS.SHA256(payload).toString(); // Hashear el nuevo payload
    savePayloadToFirestore(username, hashedPayload);
    triggerQRExpireFunction(hashedPayload); // Llamar a la función qr_expire para el nuevo payload
    document.getElementById("qr-code").src = `https://api.qrserver.com/v1/create-qr-code/?size=300x300&data=${hashedPayload}`;
  }, 10000);
}

// Guardar el payload en Firestore para compararlo más tarde.
async function savePayloadToFirestore(username, payload) {
  try {
    await axios.post('https://us-central1-tfg-borjadominguez.cloudfunctions.net/qr_check', {
      action: 'savePayload',
      username,
      payload
    });
    console.log('Payload guardado en Firestore.');
  } catch (error) {
    console.error('Error al guardar el payload en Firestore:', error);
  }
}

// Generar el payload con usuario, fecha y 8 caracteres aleatorios.
function generatePayload(username) {
  const now = new Date();
  const year = now.getFullYear();
  const month = String(now.getMonth() + 1).padStart(2, '0');
  const day = String(now.getDate()).padStart(2, '0');
  const hours = String(now.getHours()).padStart(2, '0');
  const minutes = String(now.getMinutes()).padStart(2, '0');
  const seconds = String(now.getSeconds()).padStart(2, '0');
  const dateString = `${year}${month}${day}${hours}${minutes}${seconds}`;

  const randomString = Math.random().toString(36).substr(2, 8).toUpperCase();

  return `${username}_${dateString}_${randomString}`;
}

initBlobs();
setupRegisterForm();

