# altstore-beta

A modified version of Altstore apps.json. Containing only beta apps
and making them visible to non-paterons.

Updated hourly.

To use this repository, add `https://nickchan.lol/altstore-beta/apps.json`
to your repository list in AltStore

If your version of AltStore only allow 'trusted' sources, open 
this URL in Safari: 
`altstore://source?url=https://nickchan.lol/altstore-beta/apps-safe.json`

This repository does not host AltStore beta apps. It merely 
contains instructions for AltStore to download them from 
[AltStore CDN](https://cdn.altstore.io).

The contents of this repository are licensed under the MIT
license, with the exception of `apps.json` and `apps-safe.json`,
which are licensed under AGPLv3 (see [LICENSE](LICENSE))

## Patch

It appears that in some versions of AltStore 2.0 beta, the code is updated
to not even refresh AltStore itself when patreon is not detected, so a new
approach is probably needed.
