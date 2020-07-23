import { Component, OnInit } from '@angular/core';
import { EspService, File } from '../esp.service';
import { Observable } from 'rxjs';

@Component({
  selector: 'app-flash-arduino',
  templateUrl: './flash-arduino.component.html',
  styleUrls: ['./flash-arduino.component.scss']
})
export class FlashArduinoComponent implements OnInit {
  public displayedColumns = ["name", "size", "fileOps", "flashOps"];
  public files$: Observable<File[]>;

  constructor(public espService: EspService) {
    this.files$ = espService.files$.asObservable();
  }

  ngOnInit(): void {
  }

  verifyFile(file: File): void {
    this.espService.verifyFile(file.name);
  }

  deleteFile(file: File): void {
    this.espService.deleteFile(file.name);
  }

  verifyFlash(file: File): void {
    this.espService.verifyFlash(file.name);
  }

  programFlash(file: File): void {
    this.espService.programFlash(file.name);
  }

  onFileSelected(event: Event) {
    const inputNode: any = document.querySelector('#file');
    if (typeof (FileReader) !== 'undefined') {
      var file = inputNode.files[0];
      var xhr = new XMLHttpRequest();
      var url = '/api/fileUpload';
      xhr.open('POST', url, true);
      var fd = new FormData();
      fd.append('file', file)
      xhr.send(fd);
    }
  }
}
