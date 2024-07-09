// Importa las librerías necesarias
const express = require('express'); // Framework para construir aplicaciones web y APIs
const cors = require('cors'); // Middleware para habilitar CORS (Cross-Origin Resource Sharing)
const { Firestore } = require('@google-cloud/firestore'); // Cliente para Firestore de Google Cloud

// Inicializa la aplicación Express
const app = express();

// Inicializa el cliente de Firestore
const firestore = new Firestore();

// Configura el middleware CORS y para parsear JSON
app.use(cors({ origin: true })); // Habilita CORS para todas las solicitudes
app.use(express.json()); // Parsea el cuerpo de las solicitudes como JSON

// Define la ruta para la solicitud POST en el endpoint raíz
app.post('/', async (req, res) => {
  const { payload } = req.body; // Extrae el payload del cuerpo de la solicitud

  // Verifica si el payload ha sido proporcionado
  if (!payload) {
    return res.status(400).json({ error: 'No payload provided' }); // Responde con un error si no se proporciona el payload
  }

  console.log(`Received payload: ${payload}`); // Imprime el payload recibido en la consola

  // Espera 20 segundos antes de continuar
  await new Promise(resolve => setTimeout(resolve, 20000));

  // Intentar eliminar el payload de Firestore
  try {
    const docRef = firestore.collection('payloads').doc('webPayload'); // Referencia al documento en Firestore
    const doc = await docRef.get(); // Obtiene el documento

    // Verifica si el documento existe en Firestore
    if (!doc.exists) {
      console.log(`Payload ${payload} not found in Firestore.`); // Imprime un mensaje si no se encuentra el payload
      return res.status(404).json({ message: 'Payload not found' }); // Responde con un error si no se encuentra el payload
    }

    const data = doc.data(); // Obtiene los datos del documento
    if (data && data[payload]) { // Verifica si el payload está presente en los datos
      await docRef.update({
        [payload]: Firestore.FieldValue.delete() // Elimina el payload específico del documento
      });
      console.log(`Payload ${payload} deleted from Firestore.`); // Imprime un mensaje si el payload se elimina exitosamente
      return res.status(200).json({ message: 'Payload deleted successfully' }); // Responde con un mensaje de éxito
    } else {
      console.log(`Payload ${payload} not found in Firestore.`); // Imprime un mensaje si no se encuentra el payload
      return res.status(404).json({ message: 'Payload not found' }); // Responde con un error si no se encuentra el payload
    }
  } catch (error) {
    console.error('Error deleting payload from Firestore:', error); // Registra el error en la consola
    return res.status(500).json({ error: 'Internal server error' }); // Responde con un error interno del servidor
  }
});

// Exporta la aplicación para ser usada como función en la nube
exports.qr_expire = app;
