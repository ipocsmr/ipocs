import { Component, ComponentRef } from '@angular/core';
import { Overlay, OverlayConfig } from '@angular/cdk/overlay';
import { EspService } from './esp.service';
import { Router, NavigationEnd, ActivatedRoute } from '@angular/router';
import { Title } from '@angular/platform-browser';
import { filter, map } from 'rxjs/operators';
import { Version } from '../version'
import { ComponentPortal } from '@angular/cdk/portal';
import { SpinnerComponent } from './spinner/spinner.component';
import { ProgressComponent } from './progress/progress.component';
import { Observable } from 'rxjs';
import { Mode } from './mode.enum';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  public title: string;
  public version: string = Version;
  private spinnerOverlayRef = this.overlay.create(new OverlayConfig({
    hasBackdrop: true,
    backdropClass: 'dark-backdrop',
    positionStrategy: this.overlay.position().global().centerHorizontally().centerVertically()
  }));
  private portalRef: ComponentRef<ProgressComponent> = undefined;
  public Mode = Mode;

  private overlayRef = this.overlay.create(new OverlayConfig({
    hasBackdrop: false,
    width: '80%',
    positionStrategy: this.overlay.position().global().centerHorizontally().bottom("80px")
  }));
  public mode$: Observable<Mode>;

  constructor(private router: Router, private activatedRoute: ActivatedRoute, private titleService: Title,
              private overlay: Overlay, private espService: EspService) {
    this.mode$ = espService.mode$.asObservable();
  }

  ngOnInit() {
    const appTitle = this.titleService.getTitle();
    this.router
      .events.pipe(
        filter(event => event instanceof NavigationEnd),
        map(() => {
          const child = this.activatedRoute.firstChild;
          if (child.snapshot.data['title']) {
            return appTitle + " - " + child.snapshot.data['title'];
          }
          return appTitle;
        })
      ).subscribe((ttl: string) => {
        this.title = ttl;
        this.titleService.setTitle(ttl);
      });
    this.espService.connected$.subscribe((isConnected) => {
      if (isConnected) {
        this.spinnerOverlayRef.detach();
      } else {
        if (!this.spinnerOverlayRef.hasAttached()) {
          const loggingPortal = new ComponentPortal(SpinnerComponent);
          this.spinnerOverlayRef.attach(loggingPortal);
        }
      }
    });
    this.espService.operationInProgress$.subscribe((inProgress) => {
      if (this.portalRef === undefined && inProgress) {
        const portal = new ComponentPortal(ProgressComponent);
        this.portalRef = this.overlayRef.attach(portal);
        this.portalRef.instance.message$.next("Operation started...");
      }
      if (this.portalRef !== undefined && !inProgress) {
        setTimeout(() => {
          this.overlayRef.detach();
          this.portalRef = undefined;
        }, 1000);
      }
    });
    this.espService.progress$.subscribe((progress) => {
      if (this.portalRef !== undefined) {
        this.portalRef.instance.progress$.next(progress);
      }
    });
    this.espService.progressMessage$.subscribe((message) => {
      if (this.portalRef !== undefined) {
        this.portalRef.instance.message$.next(message);
      }
    });
  }
}
