import { Component, OnInit, ComponentRef } from '@angular/core';
import { EspService, File } from '../esp.service';
import { Observable, interval } from 'rxjs';
import { Overlay, OverlayConfig } from '@angular/cdk/overlay';
import { ComponentPortal } from '@angular/cdk/portal';
import { ProgressComponent } from '../progress/progress.component';
import { isPromise } from '@angular/compiler/src/util';

@Component({
  selector: 'app-flash-arduino',
  templateUrl: './flash-arduino.component.html',
  styleUrls: ['./flash-arduino.component.scss']
})
export class FlashArduinoComponent implements OnInit {
  public displayedColumns = ["name", "size", "fileOps", "flashOps"];
  public files$: Observable<File[]>;
  private portalRef: ComponentRef<ProgressComponent> = undefined;

  private overlayRef = this.overlay.create(new OverlayConfig({
    hasBackdrop: false,
    width: '80%',
    positionStrategy: this.overlay.position().global().centerHorizontally().bottom("80px")
  }));
  
  constructor(public espService: EspService, private overlay: Overlay) {
    this.files$ = espService.files$.asObservable();
  }

  ngOnInit(): void {
    this.espService.verifyFileProgress$.subscribe((progress) => {
      if (this.portalRef === undefined && progress !== undefined) {
        const portal = new ComponentPortal(ProgressComponent);
        this.portalRef = this.overlayRef.attach(portal);
        this.portalRef.instance.message$.next("Verifying file...");
      }
      if (this.portalRef !== undefined && progress !== undefined) {
        this.portalRef.instance.progress$.next(progress);
      }
      if (this.portalRef !== undefined && progress === undefined) {
        this.portalRef.instance.message$.next("Done verifying");
        setTimeout(() => {
          this.overlayRef.detach();
          this.portalRef = undefined;
        }, 1000);
      }
    });
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

  onFileSelected(event: Event): void {
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
