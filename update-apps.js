if (!process.argv[5]) {
	console.error("usage: " + process.argv[1] + " <apps.json url> <news.json location> <new apps.json location> <repository url>");
	process.exit(1)
}
const fetch = require("node-fetch")
const fs = require("fs")
const news = require(process.argv[3])
fetch(process.argv[2], { headers: { "user-agent": "AltStore/1 CFNetwork/1126 Darwin/19.5.0" }})
	.then(response => response.json())
	.then(appsjson => {
		appsjson.news = news
		betaApps = []
		for (const app of appsjson.apps) {
			if (app.beta === true) {
				app.beta = false;
				app.name += " [Beta]"
				betaApps.push(app)
			}
		}
		appsjson.apps = betaApps
		appsjson.sourceURL = process.argv[5]
		appsjson.userInfo = {}
		return appsjson
	}).then(newJson => {
		fs.writeFileSync(process.argv[4],JSON.stringify(newJson,null,2))
	})
