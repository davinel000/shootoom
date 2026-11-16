# Hardware and firmware guide

Open the [unified guide](../hardware/perfboard/shootoom-system-guide.html) in a browser for both layouts, connector tables and operating instructions. GitHub shows the HTML source. Download or clone the repository to view the page locally.

For the interactive editor on Windows, install Node.js and run these commands once from the repository root:

```powershell
Set-Location tools/perfboard-editor
npm ci --ignore-scripts
npm run build
```

Then run `Start Board Viewer.cmd` from the repository root. Both boards open from the same guide. See the [current S3 sketch](../firmware/esp32-s3/0.1.5/ShootoomS3/ShootoomS3.ino) and [upload instructions](../firmware/esp32-s3/0.1.5/README.md).
