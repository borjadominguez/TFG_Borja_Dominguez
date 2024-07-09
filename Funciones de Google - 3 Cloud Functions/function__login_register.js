// Importa las librerías necesarias
const { SecretManagerServiceClient } = require('@google-cloud/secret-manager'); // Cliente para gestionar secretos en Google Cloud
const express = require('express'); // Framework para construir aplicaciones web y APIs
const cors = require('cors'); // Middleware para habilitar CORS (Cross-Origin Resource Sharing)
const bcrypt = require('bcryptjs'); // Librería para encriptar contraseñas

// Inicializa la aplicación Express
const app = express();

// Inicializa el cliente para el Secret Manager de Google Cloud
const secretManagerClient = new SecretManagerServiceClient();

// Configura el middleware CORS y para parsear JSON
app.use(cors({ origin: true })); // Habilita CORS para todas las solicitudes
app.use(express.json()); // Parsea el cuerpo de las solicitudes como JSON

// Define los identificadores del proyecto y secreto en Google Cloud
const projectId = 'tfg-borjadominguez';
const secretId = 'user-credentials';
const secretName = `projects/${projectId}/secrets/${secretId}`;

// Función para acceder a la versión más reciente de un secreto en Google Cloud
async function accessSecretVersion(name) {
  const [version] = await secretManagerClient.accessSecretVersion({
    name: name,
  });
  return JSON.parse(version.payload.data.toString('utf8'));
}

// Función para añadir una nueva versión de un secreto en Google Cloud
async function addSecretVersion(name, payload) {
  await secretManagerClient.addSecretVersion({
    parent: name,
    payload: {
      data: Buffer.from(JSON.stringify(payload), 'utf8'),
    },
  });
}

// Función para inicializar el secreto en Google Cloud si no existe
async function initializeSecret() {
  try {
    await accessSecretVersion(`${secretName}/versions/latest`); // Intenta acceder a la versión más reciente del secreto
  } catch (e) {
    if (e.code === 5) { // Si el secreto no existe (código de error 5)
      await secretManagerClient.createSecret({
        parent: `projects/${projectId}`,
        secret: {
          name: secretId,
          replication: {
            automatic: {},
          },
        },
        secretId: secretId,
      });
      await addSecretVersion(secretName, {}); // Añade una versión inicial vacía al secreto
    } else {
      throw e; // Lanza otros errores para su manejo posterior
    }
  }
}

// Define la ruta para la solicitud POST en el endpoint raíz
app.post('/', async (req, res) => {
  const { action, username, password } = req.body; // Extrae los datos de la solicitud

  await initializeSecret(); // Inicializa el secreto

  try {
    const users = await accessSecretVersion(`${secretName}/versions/latest`); // Accede a la versión más reciente de los usuarios registrados

    if (action === 'register') {
      if (users[username]) {
        res.status(400).json({ message: 'El usuario ya está registrado.' });
      } else {
        const hashedPassword = await bcrypt.hash(password, 10); // Encripta la contraseña
        users[username] = hashedPassword;
        await addSecretVersion(secretName, users); // Guarda el usuario
        res.status(200).json({ message: 'Usuario registrado exitosamente.' });
      }
    } else if (action === 'login') {
      const hashedPassword = users[username];
      if (hashedPassword && await bcrypt.compare(password, hashedPassword)) {
        res.status(200).json({ message: 'Inicio de sesión exitoso.' });
      } else {
        res.status(400).json({ message: 'Usuario o contraseña incorrecta.' });
      }
    } else {
      res.status(400).json({ message: 'Acción no válida.' });
    }
  } catch (error) {
    console.error('Error handling request', error);
    res.status(500).json({ message: 'Internal Server Error' });
  }
});

// Exporta la aplicación para ser usada como función en la nube
exports.functionLoginRegister = app;
