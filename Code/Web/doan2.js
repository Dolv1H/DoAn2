

// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyAjW1MW2lD7JvJYVJ645Fw6S-0JEdkffsY",
  authDomain: "doan2-008.firebaseapp.com",
  databaseURL: "https://doan2-008-default-rtdb.firebaseio.com",
  projectId: "doan2-008",
  storageBucket: "doan2-008.appspot.com",
  messagingSenderId: "1078234037365",
  appId: "1:1078234037365:web:53c3e94729fe99cd8e8c3c",
  measurementId: "G-E0N925B311"
};

// Khởi tạo ứng dụng Firebase
firebase.initializeApp(firebaseConfig);
const drop = document.getElementsByClassName("dropdown-content");
const arrow = document.getElementById("arrow")
const rain = document.getElementById("rain")
const one = document.getElementById("one")
const two = document.getElementById("two")
const three = document.getElementById("three")
const four = document.getElementById("four")
const five = document.getElementById("five")
const cay = document.getElementById("cay")
// Auto load Temperature-------------------------
firebase.database().ref("/TT_IoT/nhietdo").on("value", function (snapshot) {
  var nd = snapshot.val();
  document.getElementById("nhietdo").innerHTML = nd;
  console.log(nd);
});
firebase.database().ref("/TT_IoT/doam").on("value", function (snapshot) {
  var nd = snapshot.val();
  document.getElementById("doam").innerHTML = nd;
  console.log(nd);
});
firebase.database().ref("/TT_IoT/anhsang").on("value", function (snapshot) {
  var nd = snapshot.val();
  document.getElementById("anhsang").innerHTML = nd;
  console.log(nd);
});
firebase.database().ref("/TT_IoT/doamdat").on("value", function (snapshot) {
  var nd = snapshot.val();
  document.getElementById("doamdat").innerHTML = nd;
  console.log(nd);
});
firebase.database().ref("/TT_IoT/cambienmua").on("value", function (snapshot) {
  var nd = snapshot.val();
  console.log(snapshot.val());
  var mua = snapshot.val();
  if (mua == "1") {
    rain.innerHTML = "Đang hoạt động";
  }
  else {
    rain.innerHTML = "Không hoạt động";
  }
});

firebase.database().ref("/TT_IoT/cambienmua").on("value", function (snapshot) {
  var nd = snapshot.val();
  console.log(snapshot.val());
  var mua = snapshot.val();
  if (mua == "1") {
    rain.innerHTML = "Không có mưa";
  }
  else {
    rain.innerHTML = "Trời đang mưa";
  }
});

firebase.database().ref("/TT_IoT/trangthairelay").on("value", function (snapshot) {
  var nd = snapshot.val();
  console.log(snapshot.val());
  var bom = snapshot.val();
  if (bom == "1") {
    maybom.innerHTML = "Đang hoạt động";
  }
  else {
    maybom.innerHTML = "Không hoạt động";
  }
});

arrow.onclick = function () {
  for (var i = 0; i < drop.length; i++) {
    var style = window.getComputedStyle(drop[i]);
    if (style.display === "none") {
      drop[i].style.display = "block";
    } else {
      drop[i].style.display = "none";
    }
  }
}
one.onclick = function () {
  firebase.database().ref("/TT_IoT/changeID").update({
    ID: "1"
  }) 
  cay.innerHTML="Đã tắt máy bơm"
}
two.onclick = function () {
  firebase.database().ref("/TT_IoT/changeID").update({
    ID: "2"
  }) 
  cay.innerHTML="Đậu xanh"
}

three.onclick = function () {
  firebase.database().ref("/TT_IoT/changeID").update({
    ID: "3"
  }) 
  cay.innerHTML="Cây cải"
}

four.onclick = function () {
  firebase.database().ref("/TT_IoT/changeID").update({
    ID: "4"
  }) 
  cay.innerHTML="Dưa hấu"
}

five.onclick = function () {
  firebase.database().ref("/TT_IoT/changeID").update({
    ID: "5"
  }) 
  cay.innerHTML="Cà chua"
}