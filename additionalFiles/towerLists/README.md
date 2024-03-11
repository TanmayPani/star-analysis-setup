# tower lists documentation
*this will be updated as new lists are created..*

# TowerLists

## Run 12 pp (200 GeV):
### Dead Towers
```
Y2012_DeadTowers.txt
```

### Secondary sets of Bad Towers
```
Y2012_AltBadTowers_155_ALT.txt
Y2012_BadTowers_Rag.txt
Y2012_BadTowers_P12id.txt
```

### Primary sets of Bad Towers
These sets are dependent on the constituent cuts: 0.2, 1.0, and 2.0 GeV and should be used
```
Y2012_BadTowers_P12id_200MeV.txt   
Y2012_BadTowers_P12id_1000MeV.txt
Y2012_BadTowers_P12id_2000MeV.txt
```



## Run 14 AuAu (200 GeV):
### Dead Towers
```
Y2014_DeadTowers_P18ih.txt
```

### Secondary sets of Bad Towers
```
Y2014_BadTowers_P18ih.txt
```

### Primary sets of Bad Towers
These sets are dependent on the constituent cuts: 0.2, 1.0, and 2.0 GeV and should be used
```
Y2014_BadTowers_P18ih_200MeV.txt
Y2014_BadTowers_P18ih_1000MeV.txt
Y2014_BadTowers_P18ih_2000MeV.txt
```


## Run 16 AuAu (200 GeV):
There are no towers for the current picoDst production of Au--Au data from Run16


## Author
**Joel Mazer**

## Notes
The "Y2014_BadTowers_P18ih.txt" and "Y2012_BadTowers_P12id.txt" lists are generic and should be the secondary lists to use if one decides to use some weird constituent cut value, else you can generate a list for you specific cut
