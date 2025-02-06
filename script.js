const SENSOR_CHANNEL_ID = '2831041';
const SENSOR_READ_API_KEY = 'H2OM7NLFQNX8D0KU';
const COMMAND_CHANNEL_ID = '2831041';
const COMMAND_WRITE_API_KEY = '7BNW1EZMDWQ0ZEOZ';
const COMMAND_READ_API_KEY = 'H2OM7NLFQNX8D0KU';

document.getElementById('commandForm').addEventListener('submit', async (e) => {
  e.preventDefault();
  
  const device_id = document.getElementById('device').value;
  const command = document.getElementById('command').value;
  
  // نستخدم الحقل field1 لتحديد الأمر و field2 لرقم الجهاز
  const url = `https://api.thingspeak.com/update.json?api_key=${COMMAND_WRITE_API_KEY}&field1=${encodeURIComponent(command)}&field2=${encodeURIComponent(device_id)}`;
  
  try {
    const response = await fetch(url);
    // نتوقع أن ترجع ThingSpeak نصًا وليس JSON
    const resultText = await response.text();
    // إذا كانت النتيجة "0" فهذا يعني حدوث خطأ (مثلاً تجاوز حد التحديث)
    if(resultText !== "0") {
      alert('تم إرسال الأمر بنجاح!');
      loadCommandHistory();
    } else {
      alert('حدث خطأ أثناء إرسال الأمر! ربما تم تجاوز حد التحديث (15 ثانية).');
    }
  } catch (error) {
    console.error('Error:', error);
    alert('حدث خطأ أثناء الإرسال!');
  }
});


// تحميل أحدث بيانات الحساس من قناة الحساس
async function updateSensorData() {
  const url = `https://api.thingspeak.com/channels/${SENSOR_CHANNEL_ID}/feeds/last.json?api_key=${SENSOR_READ_API_KEY}`;
  try {
    const response = await fetch(url);
    const data = await response.json();
    if(data && data.field1 && data.created_at) {
      document.getElementById('sensorData').innerHTML = `
        القيمة: ${data.field1} <br>
      `;
    } else {
      document.getElementById('sensorData').innerHTML = 'لا توجد بيانات حالية';
    }
  } catch (error) {
    console.error('Error fetching sensor data:', error);
  }
}

async function loadCommandHistory() {
  const url = `https://api.thingspeak.com/channels/${COMMAND_CHANNEL_ID}/feeds.json?results=10&api_key=${COMMAND_READ_API_KEY}`;
  try {
    const response = await fetch(url);
    const data = await response.json();
    if(data && data.feeds) {
      let historyHTML = '<ul>';
      data.feeds.forEach(feed => {
        historyHTML += `<li>
          [${new Date(feed.created_at).toLocaleString()}] 
          الجهاز ${feed.field2}: ${feed.field1}
        </li>`;
      });
      historyHTML += '</ul>';
      document.getElementById('commandHistory').innerHTML = historyHTML;
    } else {
      document.getElementById('commandHistory').innerHTML = 'لا توجد أوامر';
    }
  } catch (error) {
    console.error('Error loading command history:', error);
  }
}

setInterval(() => {
  updateSensorData();
  loadCommandHistory();
}, 5000);
updateSensorData();
loadCommandHistory();
