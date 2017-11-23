#!/bin/bash

rm -f viewer.data

sqlite3 viewer.data << EOF
CREATE TABLE Fonts(Id INTEGER, Name TEXT, Data BLOB);

INSERT INTO Fonts
  (Id,Name,Data) 
VALUES
  ( 0, 'Comic Jens',       ReadFile('./fonts/comic-jens/ComicJensFreePro-Regular.ttf')), 
  ( 1, 'Exo Black Italic', ReadFile('./fonts/exo/Exo-Black-Italic.otf')), 
  ( 2, 'Exo Black',        ReadFile('./fonts/exo/Exo-Black.otf')), 
  ( 3, 'Exo Italic',       ReadFile('./fonts/exo/Exo-Regular-Italic.otf')), 
  ( 4, 'Exo',              ReadFile('./fonts/exo/Exo-Regular.otf')), 
  ( 5, 'Infini Bold',      ReadFile('./fonts/infini/infini-gras.otf')), 
  ( 6, 'Infini Italic',    ReadFile('./fonts/infini/infini-italique.otf')), 
  ( 7, 'Lobster',          ReadFile('./fonts/lobster/Lobster.otf')), 
  ( 8, 'Odin Bold Italic', ReadFile('./fonts/odin-rounded/Odin Rounded - Bold Italic.otf')), 
  ( 9, 'Odin Bold',        ReadFile('./fonts/odin-rounded/Odin Rounded - Bold.otf')), 
  (10, 'Pecita',           ReadFile('./fonts/pecita/Pecita.otf')), 
  (11, 'Prida',            ReadFile('./fonts/prida65/Prida65.otf'));
EOF

