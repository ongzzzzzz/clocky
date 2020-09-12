/* eslint-disable promise/no-nesting */
/* eslint-disable promise/always-return */
const functions = require('firebase-functions');
const admin = require('firebase-admin');
const fetch = require('node-fetch');

admin.initializeApp();
let rtdb = admin.database();

// TODO: get weather from accuweather
//TODO: test these functions, change back 1 hour after u test
// TODO: get these data in clocky
//TODO: update webapp  

// let date = new Date().toLocaleString('en-US', { timeZone: 'Asia/Kuala_Lumpur' });
// console.log(`Malaysia time now is ${date}`);



function checkStatus(res) {
    if (res.ok) { // res.status >= 200 && res.status < 300
        return res.json();
    } else {
        throw new Error(res.statusText);
    }
}
//exports.updateData = functions.pubsub.schedule('every 30 minutes').onRun((context) => {
exports.updateData = functions.pubsub.schedule('every 24 hours').onRun((context) => {
    fetch('http://dataservice.accuweather.com/currentconditions/v1/234975?apikey=HuptOIEbq28g99YeSniD2juuFb8a2Btj')
        .then(checkStatus)
        .then(json => {
            console.log(JSON.stringify(json));
            if(json.length) return rtdb.ref('calendar/weather').set({
                HasPrecipitation:json[0].HasPrecipitation,
                IsDayTime:json[0].IsDayTime,
                PrecipitationType:json[0].PrecipitationType,
                Temperature:json[0].Temperature,
                WeatherText:json[0].WeatherText,
            }).then(_=>{console.log('WOOHOO I UPDATED THE WEATHER BOSS');});



            
            let time = new Date(0).setUTCSeconds(json[0].EpochTime);
            //i liek that YYYY-MM-DD format
            var dateRN = time.getFullYear() + "-" + (("0" + (time.getMonth() + 1)).slice(-2)) + "-" + (("0" + time.getDate()).slice(-2));
            //le very epico format, the hh:mm:ss format
            let minutes = time.getMinutes() < 10 ? "0"+time.getMinutes() : time.getMinutes();
            let seconds = time.getSeconds() < 10 ? "0"+time.getSeconds() : time.getSeconds();
            var timeRN = time.getHours() + ":" + minutes + ":" + seconds;

            rtdb.ref('calendar/accuracy').set({
                dateNow: dateRN,
                timeNow: timeRN,
            }).then(_=>console.log('WOOHOO I SET THE TIME BOSS')).catch(e=>{console.log(e)});

            return null;

        })
        .catch(e => {
            console.log(e);
        });
    
    return null;
})



// // Create and Deploy Your First Cloud Functions
// // https://firebase.google.com/docs/functions/write-firebase-functions
//
// exports.helloWorld = functions.https.onRequest((request, response) => {
//   functions.logger.info("Hello logs!", {structuredData: true});
//   response.send("Hello from Firebase!");
// });



    // let minutes = date.getMinutes() < 10 ? "0"+date.getMinutes() : date.getMinutes();
    // let seconds = date.getSeconds() < 10 ? "0"+date.getSeconds() : date.getSeconds();

    // let dateRN = date.getFullYear() + "-" + (("0" + (date.getMonth() + 1)).slice(-2)) + "-" + (("0" + date.getDate()).slice(-2));
    // let timeRN = date.getHours() + ":" + minutes + ":" + seconds;

    // let accurator = {}; accurator["dateNow"] = dateRN; accurator["timeNow"] = timeRN;

    // rtdb.ref('calendar/accuracy').set(accurator).catch(e => console.log(e));