// Importa las librerías necesarias
const { Firestore } = require('@google-cloud/firestore'); // Cliente para Firestore de Google Cloud
const express = require('express'); // Framework para construir aplicaciones web y APIs
const cors = require('cors'); // Middleware para habilitar CORS (Cross-Origin Resource Sharing)

// Inicializa la aplicación Express
const app = express();

// Inicializa el cliente de Firestore
const firestore = new Firestore();

// Configura el middleware CORS y para parsear JSON
app.use(cors({ origin: true })); // Habilita CORS para todas las solicitudes
app.use(express.json()); // Parsea el cuerpo de las solicitudes como JSON

// Define la ruta para la solicitud POST en el endpoint raíz
app.post('/', async (req, res) => {
  const { action, username, payload } = req.body; // Extrae los datos de la solicitud

  try {
    if (action === 'savePayload') {
      
      // Guardar el payload en Firestore
      const webPayloadRef = firestore.collection('payloads').doc('webPayload'); // Referencia al documento en Firestore
      const doc = await webPayloadRef.get(); // Obtiene el documento
      let data = doc.exists ? doc.data() : {}; // Si el documento existe, obtiene sus datos, de lo contrario, crea un objeto vacío
      data[username] = payload; // Asigna el payload al usuario
      await webPayloadRef.set(data); // Guarda los datos actualizados en Firestore
      res.status(200).json({ message: 'Payload guardado exitosamente.' }); // Respuesta de éxito
    } else if (action === 'comparePayload') {
      
      // Comparar el payload con el guardado en Firestore
      const webPayloadRef = firestore.collection('payloads').doc('webPayload'); // Referencia al documento en Firestore
      const doc = await webPayloadRef.get(); // Obtiene el documento

      if (!doc.exists) {
        res.status(400).json({ message: 'No hay payload guardado para comparar.' }); // Si no existe el documento, responde con un error
      } else {
        const data = doc.data(); // Obtiene los datos del documento
        let match = false; // Inicializa la variable de coincidencia
        for (const [key, value] of Object.entries(data)) { // Itera sobre las entradas del objeto de datos
          if (value === payload) { // Si el payload coincide
            match = true; // Marca como coincidencia
            delete data[key]; // Borra el campo específico después de la comparación
            break; // Sale del bucle
          }
        }
        if (match) {
          await webPayloadRef.set(data); // Actualiza el documento en Firestore
          res.status(200).send('success'); // Respuesta de éxito
        } else {
          res.status(200).send('incorrect'); // Respuesta de error de coincidencia
        }
      }
    } else {
      res.status(400).json({ message: 'Acción no válida.' }); // Respuesta de acción no válida
    }
  } catch (error) {
    console.error('Error handling request', error); // Registra el error en la consola
    res.status(500).json({ message: 'Internal Server Error' }); // Respuesta de error interno del servidor
  }
});

// Exporta la aplicación para ser usada como función en la nube
exports.qr_check = app;
