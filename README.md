# ImmersiveHost

ImmersiveHost is an attempt to host Immersive mode applications ("Windows Store applications", "Metro applications", "Modern UI applications", "UWP applications", etc.) in a more open context outside of the usual app container/package situations.

This was an experiment around March of 2016, and was developed referencing mainly TH2 (10586/1511) and then-contemporary RS1/1607 releases of Windows 10. It also specifically targeted only a small set of applications, and probably would take some effort to get working again.

Intent to continue does exist, but is kind of a low priority for the time being, so this is mainly useful as a case-study into the Immersive application framework on Windows, of which remarkably little documentation exists.

## Random screenshots

![Steam overlay on an example app](https://pbs.twimg.com/media/CcsoCJ9WoAAb3O_.jpg:large)

![ROTTR main menu](https://pbs.twimg.com/media/Cc3mbEGWIAAbK28.jpg:large)

![ROTTR running](https://pbs.twimg.com/media/Cc43J26WoAE7ehn.jpg:large)

![GOW:UE](https://pbs.twimg.com/media/Cc-rhL5UIAIH5CJ.jpg:large)

![Window management issues that kind of made me give up](https://pbs.twimg.com/media/CdiGIQgXIAIoOLl.jpg:large)

## Notes

The `vendor/` folder is missing as it kind of used broken dependency management and didn't really track commit hashes - a list of vendor trees is shown below:

* `vendor/minhook/`: [TsudaKageyu/minhook](https://github.com/TsudaKageyu/minhook) @ `d675576d4a2c033f0a3062935bcb19bb824c43cf`
* `vendor/pugixml/`: [zeux/pugixml](https://github.com/zeux/pugixml) @ `8b60bbdce428af065f5501fbedd3de3c426073e4`
* `vendor/udis86/`: [vmt/udis86](https://github.com/vmt/udis86) @ `56ff6c87c11de0ffa725b14339004820556e343d` (presumably - this one was copied from the Cfx build tree)