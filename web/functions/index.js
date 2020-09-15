/* eslint-disable promise/no-nesting */
/* eslint-disable promise/always-return */

const functions = require('firebase-functions');
const admin = require('firebase-admin');
const fetch = require('node-fetch');

admin.initializeApp();
let rtdb = admin.database();

//TODO: get these data in clocky
//TODO: how to adjust RTC in clocky?
//TODO: add customizable timezone, add data in firebase and add in milliseconds date init there
//TODO: update webapp  

//DONE: get weather from accuweather
//DONE: test these functions, change back 1 hour after u test

// let date = new Date().toLocaleString('en-US', { timeZone: 'Asia/Kuala_Lumpur' });
// console.log(`Malaysia time now is ${date}`);

function checkStatus(res) {
    if (res.ok) { // res.status >= 200 && res.status < 300
        return res.json();
    } else {
        throw new Error(res.statusText);
    }
}

exports.updateData = functions.pubsub.schedule('every 45 minutes').onRun((context) => {
    fetch('http://dataservice.accuweather.com/currentconditions/v1/234975?apikey=HuptOIEbq28g99YeSniD2juuFb8a2Btj')
        .then(checkStatus)
        .then(json => {
            console.log(`JSON data received: ${JSON.stringify(json)}`);

            if(json.length){
                //init a new datetime, epochtime in milliseconds
                //add timezone offset too

                let timeZone = "+0800";

                rtdb.ref('calendar/timeZone').once('value').then((snapshot) => {
                    console.log(`The timeZone received: ${snapshot.val()}`);
                    timeZone = snapshot.val();
                }).catch(e => console.log(e));
            
                var timeZoneHour = parseInt(timeZone.substring(1, 3));
                var timeZoneMinutes = parseInt(timeZone.substring(3, 5));
                var timeZoneMilliSecondsOffset = (timeZoneHour*3600 + timeZoneMinutes*60) * 1000;
                console.log(`tzHour: ${timeZoneHour}, tzMinute: ${timeZoneMinutes}, tzmsOffset: ${timeZoneMilliSecondsOffset}`);

                let time = new Date(((json[0].EpochTime)*1000) + timeZoneMilliSecondsOffset);
                //epic YYYY-MM-DD format
                var dateRN = time.getFullYear() + "-" + (("0" + (time.getMonth() + 1)).slice(-2)) + "-" + (("0" + time.getDate()).slice(-2));
                //le hh:mm:ss format
                let minutes = time.getMinutes() < 10 ? "0"+time.getMinutes() : time.getMinutes();
                let seconds = time.getSeconds() < 10 ? "0"+time.getSeconds() : time.getSeconds();
                var timeRN = time.getHours() + ":" + minutes + ":" + seconds;

                

                //set the weather first
                return rtdb.ref('calendar/weather').set({
                    weatherNow:json[0].WeatherText,
                    hasPrecipitation:json[0].HasPrecipitation,
                    dayTimeOrNot:json[0].IsDayTime,
                    precipitationType:json[0].PrecipitationType,
                    tempCelsius:json[0].Temperature.Metric.Value,
                    tempFarenheit:json[0].Temperature.Imperial.Value,
                    epochSeconds:json[0].EpochTime,
                    lastMeasured:json[0].LocalObservationDateTime,
                }).then(()=>console.log('WOOHOO I UPDATED THE WEATHER BOSS'))
                //set the time next
                .then(() => {
                    rtdb.ref('calendar/accuracy').set({
                        dateNow: dateRN,
                        timeNow: timeRN,
                    }).then(() => console.log('WOOHOO I SET THE TIME BOSS'))
                    .catch(e => console.log(e));
                });
            }

            return null;

        }).catch(e => console.log(e));
    
    return null;
})

//[{
//"LocalObservationDateTime":"2020-09-15T18:55:00+08:00",
//"EpochTime":1600167300,
//"WeatherText":"Partly sunny",
//"WeatherIcon":3,
//"HasPrecipitation":false,
//"PrecipitationType":null,
//"IsDayTime":true,
//"Temperature": {"Metric":{"Value":30.0,"Unit":"C","UnitType":17},
//              "Imperial":{"Value":86.0,"Unit":"F","UnitType":18}},
//"MobileLink":"http://m.accuweather.com/en/my/george-town/234975/current-weather/234975?lang=en-us",
//"Link":"http://www.accuweather.com/en/my/george-town/234975/current-weather/234975?lang=en-us"
//}]


// // Create and Deploy Your First Cloud Functions
// // https://firebase.google.com/docs/functions/write-firebase-functions
//
// exports.helloWorld = functions.https.onRequest((request, response) => {
//   functions.logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });