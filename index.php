<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
  <meta charset="UTF-8">
  <title>نظام IoT باستخدام ThingSpeak</title>
  <link rel="stylesheet" href="styles.css">
</head>
<body>
  <div class="container">
    <h1>لوحة تحكم الأجهزة الذكية</h1>
  
    <div class="command-section">
      <h2>إرسال أمر جديد</h2>
      <form id="commandForm">
        <div class="form-group">
          <label for="device">اختر الجهاز:</label>
          <select id="device">
            <!-- تأكد من تطابق Device ID مع ما ستستخدمه في كود الجهاز -->
            <option value="ESP32_01">الجهاز 1 (ESP32)</option>
            <option value="ESP32_02">الجهاز 2 (ESP32)</option>
          </select>
        </div>

        <div class="form-group">
          <label for="command">اختر الأمر:</label>
          <select id="command">
            <option value="LED_ON">تشغيل الـ LED</option>
            <option value="LED_OFF">إطفاء الـ LED</option>
          </select>
        </div>

        <button type="submit">إرسال الأمر</button>
      </form>
    </div>

    <!-- قسم عرض بيانات الحساس -->
    <div class="sensor-section">
      <h2>أحدث قراءة حساس</h2>
      <div id="sensorData">
        جاري تحميل البيانات...
      </div>
    </div>

    <div class="command-history">
      <h2>سجل الأوامر</h2>
      <div id="commandHistory"></div>
    </div>
  </div>
</body>
<script src="script.js"></script>
</html>
